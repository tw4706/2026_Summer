#include "EnemyStateReact.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

EnemyStateReact::EnemyStateReact(std::weak_ptr<EnemyBase> pEnemy, float searchRadius):
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateReact::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::React);
}

void EnemyStateReact::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}
}

void EnemyStateReact::Exit()
{
}
