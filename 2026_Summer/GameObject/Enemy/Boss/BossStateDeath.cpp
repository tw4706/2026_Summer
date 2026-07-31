#include "BossStateDeath.h"
#include "BossStateIdle.h"
#include "Boss.h"

BossStateDeath::BossStateDeath(std::weak_ptr<Boss> pBoss, float searchRadius) :
	BossStateBase(pBoss, searchRadius)
{
}

void BossStateDeath::Enter()
{
	auto boss = pBoss_.lock();
	if (!boss)return;
}

void BossStateDeath::Update()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//アニメーションが終了したら
	if (boss->IsAnimationEnd())
	{
		//ボスを削除する
		boss->Destory();
	}
}

void BossStateDeath::Exit()
{
}
