#include "EnemyStateAttack.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

EnemyStateAttack::EnemyStateAttack(std::weak_ptr<EnemyBase> pEnemy,float searchRadius):
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateAttack::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションに切り替える
	enemy->ChangeAnimation(AnimationState::Attack);

	//速度をゼロにする
	enemy->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
}

void EnemyStateAttack::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		//Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
	}
}

void EnemyStateAttack::Exit()
{
}
