#include "EnemyStateDeath.h"
#include "EnemyBase.h"

EnemyStateDeath::EnemyStateDeath(std::weak_ptr<EnemyBase> pEnemy,float searchRadius) :
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateDeath::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Death);
}

void EnemyStateDeath::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//死亡アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		//敵を削除
		enemy->Destory();
	}
}

void EnemyStateDeath::Exit()
{
}
