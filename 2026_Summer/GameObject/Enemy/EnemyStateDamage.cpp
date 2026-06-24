#include "EnemyStateDamage.h"
#include "EnemyBase.h"

EnemyStateDamage::EnemyStateDamage(std::weak_ptr<EnemyBase> pEnemy):
	EnemyStateBase(pEnemy)
{
}

void EnemyStateDamage::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyStateDamage::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}

void EnemyStateDamage::Exit()
{
}
