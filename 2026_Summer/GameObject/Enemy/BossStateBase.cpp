#include "BossStateBase.h"

BossStateBase::BossStateBase(std::weak_ptr<EnemyBase>pEnemy, float searchRadius):
	EnemyStateBase(pEnemy,searchRadius)
{
}
