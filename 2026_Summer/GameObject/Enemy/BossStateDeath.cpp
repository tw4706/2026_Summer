#include "BossStateDeath.h"

BossStateDeath::BossStateDeath(std::weak_ptr<Boss> pBoss, float searchRadius) :
	BossStateBase(pBoss, searchRadius)
{
}

void BossStateDeath::Enter()
{
}

void BossStateDeath::Update()
{
}

void BossStateDeath::Exit()
{
}
