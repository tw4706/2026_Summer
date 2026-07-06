#include "EnemyAttackSubStateAttack.h"
#include "EnemyAttackSubStateChance.h"
#include "EnemyAttackSubStateBase.h"
#include "../EnemyStateAttack.h"
#include "../EnemyBase.h"

EnemyAttackSubStateAttack::EnemyAttackSubStateAttack(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack):
	EnemyAttackSubStateBase(pEnemy, pEnemyAttack)
{
}

void EnemyAttackSubStateAttack::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションに切り替える
	enemy->SetAttackAnimationSpeed();

	//攻撃のコライダーを生成する
	enemy->CreateAttackCollider(100.0f, 50.0f);
}

void EnemyAttackSubStateAttack::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	if (enemy->IsAnimationEnd())
	{
		auto nextAttackState = std::make_shared<EnemyAttackSubStateChance>(pEnemy_, pEnemyAttack_);

		//攻撃のサブステートがある場合は次の隙状態に遷移
		if (pEnemyAttack_)
		{
			pEnemyAttack_->ChangeAttackState(nextAttackState);
		}
	}
}

void EnemyAttackSubStateAttack::Exit()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃コライダーの削除
	enemy->RemoveAttackCollider();
}
