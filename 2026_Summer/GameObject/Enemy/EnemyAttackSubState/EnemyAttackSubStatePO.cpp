#include "EnemyAttackSubStatePO.h"

EnemyAttackSubStatePO::EnemyAttackSubStatePO(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack):
	EnemyAttackSubStateBase(pEnemy, pEnemyAttack)
{
}

void EnemyAttackSubStatePO::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyAttackSubStatePO::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyAttackSubStatePO::Exit()
{
}
