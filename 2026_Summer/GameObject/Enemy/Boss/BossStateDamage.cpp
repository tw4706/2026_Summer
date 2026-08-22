#include "BossStateDamage.h"
#include "BossStateIdle.h"
#include "Boss.h"
#include "SoundManager.h"

BossStateDamage::BossStateDamage(std::weak_ptr<Boss>pBoss, float searchRadius):
	BossStateBase(pBoss,searchRadius)
{
}

void BossStateDamage::Enter()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	boss->ChangeAnimation(AnimationState::Damage);

	//ヒットSEの再生
	SoundManager::GetInstance().PlaySe(SE::EnemyHit);
}

void BossStateDamage::Update()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//アニメーションが終了したら
	if (boss->IsAnimationEnd())
	{
		//idle状態に遷移
		auto nextState = std::make_shared<BossStateIdle>(pBoss_, searchRadius_);
		boss->ChangeState(nextState);
		return;
	}
}

void BossStateDamage::Exit()
{
}
