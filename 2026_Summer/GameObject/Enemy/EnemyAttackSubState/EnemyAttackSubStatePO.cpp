#include "EnemyAttackSubStatePO.h"
#include "EnemyAttackSubStateAttack.h"
#include "../EnemyBase.h"
#include "../EnemyStateAttack.h"

namespace
{
	//攻撃に移行するフレーム
	constexpr float kTransitionTime = 15.0f;
}

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

	//攻撃アニメーションが15フレーム以上たったら
	if (enemy->GetCurrentAnimTime() >= kTransitionTime)
	{
		//攻撃のサブステートマシンを生成
		auto nextState = std::make_shared<EnemyAttackSubStateAttack>(pEnemy_, pEnemyAttack_);

		//攻撃の状態を準備段階から攻撃段階に切り替える
		if (pEnemyAttack_)
		{
			pEnemyAttack_->ChangeAttackState(nextState);
		}
	}
}

void EnemyAttackSubStatePO::Exit()
{
}
