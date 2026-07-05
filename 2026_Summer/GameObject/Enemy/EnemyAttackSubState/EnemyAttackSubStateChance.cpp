#include "EnemyAttackSubStateChance.h"
#include "../EnemyBase.h"
#include "../EnemyStateIdle.h"

EnemyAttackSubStateChance::EnemyAttackSubStateChance(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack):
	EnemyAttackSubStateBase(pEnemy,pEnemyAttack)
{
}

void EnemyAttackSubStateChance::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Chance);
	enemy->SetVelocity({ 0.0f,0.0f,0.0f });
}

void EnemyAttackSubStateChance::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	if (enemy->IsAnimationEnd())
	{
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, enemy->GetSearchRadius());
		enemy->ChangeState(nextState);
	}
}

void EnemyAttackSubStateChance::Exit()
{
}
