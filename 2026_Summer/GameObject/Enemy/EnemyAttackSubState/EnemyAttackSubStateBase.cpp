#include "EnemyAttackSubStateBase.h"
EnemyAttackSubStateBase::EnemyAttackSubStateBase(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack):
	pEnemy_(pEnemy),
	pEnemyAttack_(pEnemyAttack)
{
}
