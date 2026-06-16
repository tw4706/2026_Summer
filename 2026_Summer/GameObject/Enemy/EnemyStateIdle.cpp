#include "EnemyStateIdle.h"
#include "EnemyBase.h"

EnemyStateIdle::EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy):
	EnemyStateBase(pEnemy)
{
}

void EnemyStateIdle::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Idle);
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyStateIdle::Exit()
{
}
