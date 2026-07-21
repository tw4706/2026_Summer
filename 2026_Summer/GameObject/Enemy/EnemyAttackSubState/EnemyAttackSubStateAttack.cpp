#include "EnemyAttackSubStateAttack.h"
#include "EnemyAttackSubStateChance.h"
#include "EnemyAttackSubStateBase.h"
#include "../EnemyStateIdle.h"
#include "../EnemyStateAttack.h"
#include "../EnemyBase.h"

EnemyAttackSubStateAttack::EnemyAttackSubStateAttack(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack, const AttackData& attackData) :
	EnemyAttackSubStateBase(pEnemy, pEnemyAttack, attackData)
{
}

void EnemyAttackSubStateAttack::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションに切り替える
	enemy->SetAttackAnimationSpeed();

	//攻撃のコライダーを生成する
	enemy->CreateAttackCollider(attackData_.colliderRadius_, attackData_.colliderHeight_);
}

void EnemyAttackSubStateAttack::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		//攻撃コライダーの削除
		enemy->RemoveAttackCollider();

		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, enemy->GetSearchRadius());
		enemy->ChangeState(nextState);
	}
}

void EnemyAttackSubStateAttack::Exit()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;
}
