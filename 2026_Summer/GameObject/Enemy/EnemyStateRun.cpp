#include "EnemyStateRun.h"
#include "EnemyStateIdle.h"
#include "EnemyStateAttack.h"
#include "EnemyBase.h"
#include<cmath>

namespace
{
	//敵の走りアニメーション
	const wchar_t* kEnemyRun = L"Oni|Run";

	//移動速度
	const float kMoveSpeed = 0.3f;

	//経過時間
	const float kDeltaTime = 1.0f / 60.0f;

	//線形補間の割合
	const float kRotateLerpRate = 0.1f;

	//索敵範囲の半径
	const float kDebugSearchRadius = 500.0f;
}

EnemyStateRun::EnemyStateRun(std::weak_ptr<EnemyBase> pEnemy) :
	EnemyStateBase(pEnemy)
{
}

void EnemyStateRun::Enter()
{
	auto pEnemy = pEnemy_.lock();
	if (!pEnemy) return;

	//Runアニメーションに切り替える
	pEnemy->ChangeAnimation(AnimationState::Run, kEnemyRun);
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
		//攻撃状態に遷移
		auto nextState = std::make_shared<EnemyStateAttack>(pEnemy_);
		enemy->ChangeState(nextState);
		
		return;
	}

	//索敵の範囲に入ってなかったら
	if (PlayerSearchDistance(kDebugSearchRadius) == false&&!enemy->IsHit())
	{
		//検知範囲に入ったらRun状態へ遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_);
		enemy->ChangeState(nextState);
		return;
	}

	//敵とプレイヤーの位置を取得
	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//敵からプレイヤーまでのベクトルを計算
	Vector3 toPlayer = playerPos - enemyPos;
	toPlayer.y_ = 0.0f;

	//距離の計算
	float distance = std::sqrt(toPlayer.x_ * toPlayer.x_ + toPlayer.z_ * toPlayer.z_);

	//距離が0.1fだった場合は何も行わない
	if (distance < 0.1f)
	{
		auto nextState = std::make_shared<EnemyStateAttack>(pEnemy_);
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
	//次の目標の角度
	float nextAngle = currentAngle;

	//線形補間を用いて滑らかに回転する
	nextAngle = currentAngle + (targetAngle - currentAngle) * kRotateLerpRate;

	//計算した角度を適用
	enemy->SetMoveAngle(nextAngle);
}

void EnemyStateRun::Exit()
{
}
