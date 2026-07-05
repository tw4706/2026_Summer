#include "EnemyAttackSubStateChance.h"

EnemyAttackSubStateChance::EnemyAttackSubStateChance(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack):
	EnemyAttackSubStateBase(pEnemy,pEnemyAttack)
{
}

void EnemyAttackSubStateChance::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyAttackSubStateChance::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyAttackSubStateChance::Exit()
{
}
