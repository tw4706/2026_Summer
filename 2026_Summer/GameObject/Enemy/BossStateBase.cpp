#include "BossStateBase.h"
#include "Boss.h" 

BossStateBase::BossStateBase(std::weak_ptr<Boss>pBoss, float searchRadius):
	EnemyStateBase(pBoss,searchRadius),
	pBoss_(pBoss)
{
}