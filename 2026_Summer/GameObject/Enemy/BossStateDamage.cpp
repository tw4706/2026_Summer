#include "BossStateDamage.h"

BossStateDamage::BossStateDamage(std::weak_ptr<Boss>pBoss, float searchRadius):
	BossStateBase(pBoss,searchRadius)
{
}

void BossStateDamage::Enter()
{
	auto boss = pEnemy_.lock();
	if (!boss)return;
}

void BossStateDamage::Update()
{
	auto boss = pEnemy_.lock();
	if (!boss)return;
}

void BossStateDamage::Exit()
{
}
