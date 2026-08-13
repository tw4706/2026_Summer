#include "BossStateRangedAttack.h"
#include "BossStateRun.h"
#include "Boss.h"
#include "EnemyAttackDataLoader.h"

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//斬撃コライダーの半径
	constexpr float kSlashRadius = 100.0f;

	//斬撃が飛んでいく速度
	constexpr float kSlashSpeed = 1500.0f;

	//攻撃モーション全体の時間
	constexpr float kAttackDuration = 1.0f;
}

BossStateRangedAttack::BossStateRangedAttack(std::weak_ptr<Boss> pBoss, float searchRadius) :
	BossStateBase(pBoss, searchRadius),
	attackTime_(0.0f)
{
}

void BossStateRangedAttack::Enter()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	attackTime_ = 0.0f;

	//攻撃アニメーションへ遷移
	boss->ChangeAnimation(AnimationState::EnemyAttack);

	//ボスの攻撃力を取得して斬撃コライダーに設定
	int damage = 0;
	if (const EnemyAttackDataLoader* loader = boss->GetAttackDataLoader())
	{
		if (const AttackData* data = loader->GetAttackData(AttackType::NormalAttack))
		{
			damage = static_cast<int>(data->damage_);
		}
	}

	//飛んでいく斬撃コライダーを生成
	boss->CreateRangedAttackCollider(kSlashRadius, kSlashSpeed, damage);
}

void BossStateRangedAttack::Update()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	attackTime_ += kDeltaTime;

	//一定時間経ったらRun状態に戻る
	if (attackTime_ >= kAttackDuration)
	{
		//クールタイムをリセットしRun状態に戻る
		boss->AttackCoolDown();
		auto nextState = std::make_shared<BossStateRun>(pBoss_, searchRadius_);
		boss->ChangeState(nextState);
		return;
	}
}

void BossStateRangedAttack::Exit()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//斬撃コライダーの削除
	boss->RemoveAttackCollider();
}