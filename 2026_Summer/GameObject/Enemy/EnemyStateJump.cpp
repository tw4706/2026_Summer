#include "EnemyStateJump.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

EnemyStateJump::EnemyStateJump(std::weak_ptr<EnemyBase> pEnemy, float searchRadius):
	EnemyStateBase(pEnemy,searchRadius),
	deltaTime_(0.0f)
{
}

void EnemyStateJump::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Jump);
}

void EnemyStateJump::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy) return;
}

void EnemyStateJump::Exit()
{
}
