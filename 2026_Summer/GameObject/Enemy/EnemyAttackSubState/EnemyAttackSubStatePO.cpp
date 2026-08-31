#include "EnemyAttackSubStatePO.h"
#include "EnemyAttackSubStateAttack.h"
#include "../EnemyBase.h"
#include "../BigMan.h"
#include "../EnemyStateAttack.h"
#include "SoundManager.h"

EnemyAttackSubStatePO::EnemyAttackSubStatePO(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack, const AttackData& attackData):
	EnemyAttackSubStateBase(pEnemy, pEnemyAttack, attackData)
{
}

void EnemyAttackSubStatePO::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃前のエフェクト再生
	enemy->PlayPrevEnemyAttackEffect();

	//攻撃タイプに応じてアニメーションを切り替える
	//攻撃アニメーションをスローにして予備動作っぽくしている
	switch (attackData_.type_)
	{
	case AttackType::JumpAttack:
		enemy->ChangeAnimation(AnimationState::EnemyJumpAttack);
		if (std::dynamic_pointer_cast<BigMan>(enemy))
		{
			enemy->PlayEnemyJumpAttackRangeEffect();
		}
		break;
	case AttackType::NormalAttack:
	default:
		enemy->ChangeAnimation(AnimationState::EnemyAttack);
		break;
	}
	enemy->SetSlowAnimationSpeed();

	SoundManager::GetInstance().PlaySe(SE::EnemyPrevAttack);

}

void EnemyAttackSubStatePO::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃アニメーションがデータで取得してきたフレーム以上だった場合
	if (enemy->GetCurrentAnimTime() >= attackData_.attackTransFrame_)
	{
		//攻撃に入ると同時にBigManの場合はジャンプ攻撃エフェクトを止める
		if (std::dynamic_pointer_cast<BigMan>(enemy))
		{
			enemy->StopJumpAttackEffect();
		}

		//攻撃のサブステートマシンを生成
		auto nextState = std::make_shared<EnemyAttackSubStateAttack>(pEnemy_, pEnemyAttack_,attackData_);

		//攻撃の状態を準備段階から攻撃段階に切り替える
		if (pEnemyAttack_)
		{
			pEnemyAttack_->ChangeAttackState(nextState);
		}
	}
}

void EnemyAttackSubStatePO::Exit()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//何かしらの不具合でエフェクトが止まっていないのを防ぐためエフェクトを止めておく
	if (std::dynamic_pointer_cast<BigMan>(enemy))
	{
		enemy->StopJumpAttackEffect();
	}
}
