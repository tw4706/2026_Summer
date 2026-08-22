#include "EnemyStateDamage.h"
#include "EnemyStateRun.h"
#include "EnemyBase.h"
#include "SoundManager.h"

EnemyStateDamage::EnemyStateDamage(std::weak_ptr<EnemyBase> pEnemy, float searchRadius):
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateDamage::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Damage);

	//ヒットSEの再生
	SoundManager::GetInstance().PlaySe(SE::EnemyHit);
}

void EnemyStateDamage::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		//Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}
}

void EnemyStateDamage::Exit()
{
}
