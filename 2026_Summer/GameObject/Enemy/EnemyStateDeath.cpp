#include "EnemyStateDeath.h"
#include "EnemyBase.h"

namespace
{
	//死亡アニメーション
	const wchar_t* kEnemyDeath = L"Oni|Death";
}

EnemyStateDeath::EnemyStateDeath(std::weak_ptr<EnemyBase> pEnemy) :
	EnemyStateBase(pEnemy)
{
}

void EnemyStateDeath::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Death, kEnemyDeath);
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
