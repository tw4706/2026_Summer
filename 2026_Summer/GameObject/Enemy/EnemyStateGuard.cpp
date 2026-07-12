#include "EnemyStateGuard.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

EnemyStateGuard::EnemyStateGuard(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy, searchRadius)
{
}

void EnemyStateGuard::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//アニメーションの切り替え
	enemy->ChangeAnimation(AnimationState::Guard);
}

void EnemyStateGuard::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		//Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}
}

void EnemyStateGuard::Exit()
{
}
