#include "EnemyStateRun.h"
#include "EnemyStateIdle.h"
#include "EnemyStateAttack.h"
#include "EnemyStateReturn.h"
#include "EnemyBase.h"
#include<cmath>

namespace
{
	//移動速度
	const float kMoveSpeed = 0.3f;

	//経過時間
	const float kDeltaTime = 1.0f / 60.0f;

	//線形補間の割合
	const float kRotateLerpRate = 0.1f;

	//索敵範囲の半径
	const float kDebugSearchRadius = 500.0f;

	//攻撃開始の範囲
	const float kAttackRange = 180.0f;

	//視線の高さ(Rayで障害物の判定を行うのに使用)
	const float kEyeHeight = 50.0f;

	//2点間に障害物があるかどうかをRayで判定する
	//hitしていなければ視線が通っている(=true)
	bool HasLineOfSight(int stageModelHandle, const Vector3& from, const Vector3& to)
	{
		VECTOR start = VGet(from.x_, from.y_ + kEyeHeight, from.z_);
		VECTOR end = VGet(to.x_, to.y_ + kEyeHeight, to.z_);

		MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageModelHandle, -1, start, end);

		return hit.HitFlag == FALSE;
	}
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

	//索敵の範囲に入ってなかったら
	if (PlayerSearchDistance(kDebugSearchRadius) == false && !enemy->IsHit())
	{
		auto nextState = std::make_shared<EnemyStateReturn>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	//敵とプレイヤーの位置を取得
	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//移動先の決定
	//プレイヤーへの視線が通っているかどうかを判定
	bool hasLineOfSight = HasLineOfSight(enemy->GetStageModelHandle(), enemyPos, playerPos);

	Vector3 targetPos;

	if (hasLineOfSight)
	{
		//視線が通っている場合は経路追従をやめて直進する
		if (enemy->GetPathFollower().HasPath())
		{
			enemy->GetPathFollower().ClearPath();
		}
		targetPos = playerPos;
	}
	else
	{
		//視線が遮られている場合は経路探索を使う
		//まだ経路を持っていない場合のみ新しく探索する
		if (!enemy->GetPathFollower().HasPath())
		{
			std::vector<Vector3> path = enemy->GetPathFinder().FindPath(enemyPos, playerPos);
			enemy->GetPathFollower().SetPath(path);
		}

		//経路が見つからなかった場合は、その場で待機
		if (!enemy->GetPathFollower().HasPath())
		{
			return;
		}

		//次にいく経路上の目標を取得する
		targetPos = enemy->GetPathFollower().GetCurrentTarget(enemyPos);
	}

	//敵からプレイヤーまでのベクトル
	Vector3 toPlayer = playerPos - enemyPos;

	//ゲーム内では高さを行わないのでYを0にする
	toPlayer.y_ = 0.0f;

	//距離の計算
	float distance = toPlayer.Length();

	//距離が0.1fだった場合は何も行わない
	if (distance <= kAttackRange)
	{
		auto nextState = std::make_shared<EnemyStateAttack>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		printfDx(L"攻撃に遷移した！");
		return;
	}

	//正規化
	toPlayer.Normalize();

	//もしプレイヤーとぶつかっていない場合
	if (!enemy->IsHit())
	{
		//移動速度を設定
		Vector3 moveVec = { toPlayer.x_ * kMoveSpeed * kDeltaTime, 0.0f, toPlayer.z_ * kMoveSpeed * kDeltaTime };

		//計算した位置を適応
		Vector3 nextPos = enemyPos + moveVec;

		//位置のセット
		enemy->SetPos(nextPos);
	}

	//進行方向の角度
	float targetAngle = std::atan2f(toPlayer.x_, -toPlayer.z_);

	//現在の角度
	float currentAngle = enemy->GetMoveAngle();

	float angleDiff = targetAngle - currentAngle;

	//-πからπの範囲に正規化
	while (angleDiff > DX_PI_F)  angleDiff -= 2.0f * DX_PI_F;
	while (angleDiff < -DX_PI_F) angleDiff += 2.0f * DX_PI_F;

	//次の目標の角度を線形補間を用いて滑らかに回転する
	float nextAngle = currentAngle + angleDiff * kRotateLerpRate;

	//計算した角度を適用
	enemy->SetMoveAngle(nextAngle);
}

void EnemyStateRun::Exit()
{
}
