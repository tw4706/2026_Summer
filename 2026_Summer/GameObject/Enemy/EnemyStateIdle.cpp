#include "EnemyStateIdle.h"
#include "EnemyStateRun.h"
#include "EnemyBase.h"
namespace
{
	//敵アニメーション
	//Idle
	const wchar_t* kEnemyIdle = L"Oni|Idle";
}


EnemyStateIdle::EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy) :
	EnemyStateBase(pEnemy)
{
}

void EnemyStateIdle::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Idle, kEnemyIdle);

	enemy->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//索敵範囲かつまだ当たっていなかったら
	if (PlayerSearchDistance(200.0f)&&!enemy->IsHit())
	{
		//検知範囲に入ったらRun状態へ遷移
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_);
		enemy->ChangeState(nextState);
		return;
	}
}

void EnemyStateIdle::Exit()
{
}
