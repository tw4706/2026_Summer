#include "EnemyStateAttack.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

namespace
{
	//敵の攻撃アニメーション
	const wchar_t* kEnemyAttack = L"Oni|Attack";
}

EnemyStateAttack::EnemyStateAttack(std::weak_ptr<EnemyBase> pEnemy):
	EnemyStateBase(pEnemy)
{
}

void EnemyStateAttack::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションに切り替える
	enemy->ChangeAnimation(AnimationState::Attack, kEnemyAttack);

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
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_);
		enemy->ChangeState(nextState);
	}
}

void EnemyStateAttack::Exit()
{
}
