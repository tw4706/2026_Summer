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
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	if (PlayerSearchDistance(100.0f))
	{
		//検知範囲に入ったらRun状態へ遷移
		enemy->ChangeState(std::make_shared<EnemyStateRun>(pEnemy_));
		return;
	}
}

void EnemyStateIdle::Exit()
{
}
