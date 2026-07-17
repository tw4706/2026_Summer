#include "EnemyStateReact.h"
#include "EnemyStateIdle.h"
#include "EnemyStateRun.h"
#include "EnemyStateAttack.h"
#include "EnemyBase.h"

namespace
{
	//見渡す角度
	constexpr float kAngle = DX_PI_F / 6.0f;

	//見渡しているフレーム数
	constexpr float kReactFrame = 135.0f;

	//回転時の線形補間の割合
	constexpr float kRotateLerpRate = 0.03f;
}

EnemyStateReact::EnemyStateReact(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy, searchRadius)
{
}

void EnemyStateReact::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::React);

	//敵からプレイヤーへのベクトルを取り敵の向く角度を計算
	Vector3 toPlayer = enemy->GetPlayerPos() - enemy->GetPos();
	toPlayer.y_ = 0.0f;
	toPlayer.Normalize();

	toPlayerDir_ = toPlayer;
}

void EnemyStateReact::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//タイマーの更新
	reactTimer_++;
	enemy->moveAngle_ = RotateAngle(enemy->moveAngle_, toPlayerDir_, kRotateLerpRate);

	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();
	//障害物越しには気づかれないようにしたいので敵の視線の判定を行う
	bool hasLineOfSight = HasLineOfSight(enemy->GetStageModelHandle(), enemyPos, playerPos);

	bool isPlayerInRange = enemy->IsPlayerInRange(searchRadius_);

	if (isPlayerInRange && hasLineOfSight)
	{
		//プレイヤーを発見したら追いかける
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	//このフレーム数だけはアニメーションを行う
	if (reactTimer_ < kReactFrame)
	{
		return;
	}

	//何もない場合はIdleに遷移
	auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
	enemy->ChangeState(nextState);
}

void EnemyStateReact::Exit()
{
}
