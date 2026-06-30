#include "EnemyStateDamage.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

namespace
{
	const wchar_t* kEnemyDamage = L"Oni|Damage";
}

EnemyStateDamage::EnemyStateDamage(std::weak_ptr<EnemyBase> pEnemy, float searchRadius):
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateDamage::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Damage);
}

void EnemyStateDamage::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//アニメーションが終わったら
	if (enemy->IsAnimationEnd())
	{
		//Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

}

void EnemyStateDamage::Exit()
{
}
