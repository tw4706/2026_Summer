#include "EnemyAttackSubStatePO.h"
#include "EnemyAttackSubStateAttack.h"
#include "../EnemyBase.h"
#include "../EnemyStateAttack.h"

EnemyAttackSubStatePO::EnemyAttackSubStatePO(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack):
	EnemyAttackSubStateBase(pEnemy, pEnemyAttack)
{
}

void EnemyAttackSubStatePO::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションをスローにして予備動作っぽくしている
	enemy->ChangeAnimation(AnimationState::EnemyAttack);
	enemy->SetSlowAnimationSpeed();
}

void EnemyAttackSubStatePO::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	constexpr float kTransitionTime = 15.0f;

	if (enemy->GetCurrentAnimTime() >= kTransitionTime)
	{
		// 本攻撃サブステートを生成
		auto nextState = std::make_shared<EnemyAttackSubStateAttack>(pEnemy_, pEnemyAttack_);

		// 親ステートを本攻撃に切り替える
		if (pEnemyAttack_)
		{
			pEnemyAttack_->ChangeAttackState(nextState);
		}
	}
}

void EnemyAttackSubStatePO::Exit()
{
}
