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
	bool hasLineOfSight = HasLineOfSight(enemy->GetStageModelHandle(), enemyPos, playerPos);

	//プレイヤーが反応範囲に入っているかどうかを判定
	bool isPlayerInRange = enemy->IsPlayerInRange(searchRadius_);

	//プレイヤーが索敵範囲に入っていないまたは視線が通ってないかつ敵と当たっていなかったら
	if ((!isPlayerInRange || !hasLineOfSight) && !enemy->IsHit())
	{
		//もとの位置に戻るreturn状態に遷移
		auto nextState = std::make_shared<EnemyStateReturn>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	//ターゲット座標
	Vector3 targetPos;

	//視線が通っている場合
	if (hasLineOfSight)
	{
		//経路追従をやめて直進する
		if (enemy->pathFollower_.HasPath())
		{
			enemy->pathFollower_.ClearPath();
		}
		targetPos = playerPos;
	}
	else//視線が通らない場合(障害物などで遮られている場合)
	{
		//経路がない場合は再度探索して経路をセットする
		//A*探索で障害物を回避した経路を作り経路に沿って移動する
		if (!enemy->pathFollower_.HasPath())
		{
			std::vector<Vector3> path = enemy->pathFinder_.FindPath(enemyPos, playerPos);

			if (!path.empty())
			{
				enemy->pathFollower_.SetPath(path);
			}
		}

		//敵が次の経路を持っている場合
		if (enemy->pathFollower_.HasPath())
		{
			//次に進む位置に代入
			targetPos = enemy->pathFollower_.GetCurrentTarget(enemyPos);
		}
		else
		{
			//経路が見つからなかった場合はそのままプレイヤーに追従
			targetPos = playerPos;
		}
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
