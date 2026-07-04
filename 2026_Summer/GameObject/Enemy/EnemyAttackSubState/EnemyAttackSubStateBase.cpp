#include "EnemyAttackSubStateBase.h"
EnemyAttackSubStateBase::EnemyAttackSubStateBase(EnemyBase* pEnemy, EnemyStateAttack* pEnemyAttack):
	pEnemy_(pEnemy),
	pEnemyAttack_(pEnemyAttack)
{
}

EnemyAttackSubStateBase::~EnemyAttackSubStateBase()
{
}
