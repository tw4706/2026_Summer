#include "BossStateBase.h"

BossStateBase::BossStateBase(std::weak_ptr<Boss>pBoss, float searchRadius):
	EnemyStateBase(pEnemy_,searchRadius)
{
}