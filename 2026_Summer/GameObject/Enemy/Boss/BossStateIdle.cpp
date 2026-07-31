#include "BossStateIdle.h"
#include "BossStateRun.h"
#include "Boss.h"

BossStateIdle::BossStateIdle(std::weak_ptr<Boss> pBoss, float searchRadius):
	BossStateBase(pBoss,searchRadius)
{
}

void BossStateIdle::Enter()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//Idleó‘Ô‚É‘JˆÚ
	boss->ChangeAnimation(AnimationState::Idle);
}

void BossStateIdle::Update()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//ƒvƒŒƒCƒ„[‚ªõ“G”ÍˆÍ“à‚É‚Í‚¢‚Á‚½‚ç
	if (PlayerSearchDistance(searchRadius_))
	{
		//Runó‘Ô‚É‘JˆÚ
		auto nextState = std::make_shared<BossStateRun>(pBoss_, searchRadius_);
		boss->ChangeState(nextState);
		return;
	}
}

void BossStateIdle::Exit()
{
}
