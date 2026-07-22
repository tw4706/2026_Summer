#include "EnemyStateRun.h"
#include "EnemyStateIdle.h"
#include "EnemyStateAttack.h"
#include "EnemyStateReturn.h"
#include "EnemyBase.h"
#include "System/NavigationGrid.h"
#include<cmath>

namespace
{
	//移動速度
	constexpr float kMoveSpeed = 0.3f;

	//線形補間の割合
	constexpr float kRotateLerpRate = 0.1f;

	//攻撃開始の範囲
	constexpr float kAttackRange = 180.0f;

	//視線の高さ(Rayで障害物の判定を行うのに使用)
	constexpr float kEyeHeight = 50.0f;

	//視線の追従範囲の角度
	constexpr float kVisionAngle = DX_PI_F / 4.0f;

	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//見失って次のステートに遷移する時間
	constexpr float kLoseSightTime = 2.0f;
}

EnemyStateRun::EnemyStateRun(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy, searchRadius)
{
}

void EnemyStateRun::Enter()
{
	auto pEnemy = pEnemy_.lock();
	if (!pEnemy) return;

	//Runアニメーションに切り替える
	pEnemy->ChangeAnimation(AnimationState::Run);
}

void EnemyStateRun::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy) return;

	//敵と当たってたら
	if (enemy->IsHit())
	{
		//ヒットフラグをリセット
		enemy->ResetHitFlag();
	}

	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//プレイヤーへ視線が通っているかどうかを判定
	bool hasLineOfSight = false;

	//ターゲット位置はA*探索を用いた移動経路の計算を行った位置
	Vector3 targetPos = MoveTargetPath(enemy, enemyPos, playerPos, hasLineOfSight);

	//プレイヤーが反応範囲に入っているかどうかを判定
	bool isPlayerInRange = enemy->IsPlayerInRange(searchRadius_);

	//プレイヤーが索敵範囲に入っていないまたは視線が通ってないかつ敵と当たっていなかった
	bool isLoseSight = (!isPlayerInRange || !hasLineOfSight) && !enemy->IsHit();

	//敵を見失ったら
	if (isLoseSight)
	{
		//タイマーを進める
		loseSightTimer_ += kDeltaTime;

		//タイマーが見失って遷移する時間を超えたら
		if (loseSightTimer_ >= kLoseSightTime)
		{
			//もとの位置に戻るreturn状態に遷移
			auto nextState = std::make_shared<EnemyStateReturn>(pEnemy_, searchRadius_);
			enemy->ChangeState(nextState);
			return;
		}
	}
	else
	{
		loseSightTimer_ = 0.0f;
	}

	//ターゲットへのベクトル
	Vector3 toTarget = targetPos - enemyPos;

	//高さを使わないのでYを0にする
	toTarget.y_ = 0.0f;

	//敵からプレイヤーまでのベクトル
	Vector3 toPlayer = playerPos - enemyPos;
	toPlayer.y_ = 0.0f;
	float distance = toPlayer.Length();

	//一定距離まで来たら攻撃に遷移
	if (distance <= kAttackRange)
	{
		auto nextState = std::make_shared<EnemyStateAttack>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	//正規化
	toTarget.Normalize();

	//プレイヤーとぶつかっていない場合
	if (!enemy->IsHit())
	{
		//速度・位置の適用
		ApplyMove(enemy, enemyPos, toTarget, kMoveSpeed);
	}

	//角度を線形補間して適用
	enemy->moveAngle_ = RotateAngle(enemy->moveAngle_, toTarget, kRotateLerpRate);
}

void EnemyStateRun::Exit()
{
}
