#include "EnemyAttackSubStateAttack.h"
#include "EnemyAttackSubStateChance.h"
#include "EnemyAttackSubStateBase.h"
#include "../EnemyStateIdle.h"
#include "../EnemyStateAttack.h"
#include "../EnemyBase.h"
#include "SoundManager.h"

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

	isAttackColliderActive_ = false;

	SoundManager::GetInstance().PlaySe(SE::BossAttack);
}

void EnemyAttackSubStateAttack::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//Attackステートに入ってからの経過フレームを取得
	float currentFrame = enemy->GetCurrentAnimTime();

	//有効開始フレームに達したら攻撃コライダーを生成
	if (!isAttackColliderActive_ && currentFrame >= attackData_.attackColliderStartFrame_)
	{
		enemy->CreateAttackCollider(attackData_.colliderRadius_, attackData_.colliderHeight_,attackData_.damage_);
		isAttackColliderActive_ = true;
	}

	//有効終了フレームに達したら攻撃コライダーを削除
	if (isAttackColliderActive_ && currentFrame >= attackData_.attackColliderEndFrame_)
	{
		enemy->RemoveAttackCollider();
		isAttackColliderActive_ = false;
	}

	//アニメーションが終了したら
	if (enemy->IsAnimationEnd())
	{
		//攻撃コライダーが残っていたら削除
		if (isAttackColliderActive_)
		{
			enemy->RemoveAttackCollider();
			isAttackColliderActive_ = false;
		}
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, enemy->GetSearchRadius());
		enemy->ChangeState(nextState);
	}
}

void EnemyAttackSubStateAttack::Exit()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	if (isAttackColliderActive_)
	{
		enemy->RemoveAttackCollider();
		isAttackColliderActive_ = false;
	}
}
