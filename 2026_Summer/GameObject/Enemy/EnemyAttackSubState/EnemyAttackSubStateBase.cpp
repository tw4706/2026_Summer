#include "EnemyAttackSubStateBase.h"
EnemyAttackSubStateBase::EnemyAttackSubStateBase(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack):
	pEnemy_(pEnemy),
	pEnemyAttack_(pEnemyAttack)
{
}

EnemyAttackSubStateBase::EnemyAttackSubStateBase(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack, const AttackData& attackData): 
	pEnemy_(pEnemy),
	pEnemyAttack_(pEnemyAttack),
	attackData_(attackData)
{
}
