#include "BossStateIdle.h"

BossStateIdle::BossStateIdle(std::weak_ptr<Boss> pBoss, float searchRadius):
	BossStateBase(pBoss,searchRadius)
{
}

void BossStateIdle::Enter()
{
	auto boss = pEnemy_.lock();
	if (!boss)return;
}

void BossStateIdle::Update()
{
	auto boss = pEnemy_.lock();
	if (!boss)return;
}

void BossStateIdle::Exit()
{
}
