#include "EnemyStateIdle.h"
#include "EnemyBase.h"
namespace
{
	//敵アニメーション
	//Idle
	const wchar_t* kEnemyIdle = L"Armature|Idle";
}


EnemyStateIdle::EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy) :
	EnemyStateBase(pEnemy)
{
}

void EnemyStateIdle::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Idle, kEnemyIdle);
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyStateIdle::Exit()
{
}
