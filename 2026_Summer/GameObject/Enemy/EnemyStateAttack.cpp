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

	enemy->ChangeAnimation(AnimationState::Attack, kEnemyAttack);
	enemy->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
}

void EnemyStateAttack::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		if (enemy->IsHit())
		{
			//ヒットフラグを戻す
			enemy->ResetHitFlag();

			//ぶつかっている間はIdleに戻さずに攻撃アニメーションを継続する
			this->Enter();
			return;
		}
		//そうでない場合は
		else
		{
			//Idle状態へ遷移
			auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_);
			enemy->ChangeState(nextState);
			return;
		}
	}
}

void EnemyStateAttack::Exit()
{
}
