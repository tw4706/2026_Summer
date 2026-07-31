#include "BossStateAttack.h"
#include "BossStateRun.h"
#include "Boss.h"

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//攻撃判定の半径
	constexpr float kAttackRadius = 100.0f;
	//攻撃判定を出す距離(ボスからのオフセット)
	constexpr float kAttackDistance = 150.0f;

	//攻撃モーション全体の時間(この時間が経ったら接近状態に戻る)
	constexpr float kAttackDuration = 1.0f;
}

BossStateAttack::BossStateAttack(std::weak_ptr<Boss> pBoss, float searchRadius) :
	BossStateBase(pBoss, searchRadius),
	attackTime_(0.0f)
{
}

void BossStateAttack::Enter()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	attackTime_ = 0.0f;

	//攻撃アニメーションへ遷移
	boss->ChangeAnimation(AnimationState::EnemyAttack);

	//雑実装:攻撃コライダーを即座に生成
	boss->CreateAttackCollider(kAttackRadius, kAttackDistance);
}

void BossStateAttack::Update()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	attackTime_ += kDeltaTime;

	//一定時間経ったら接近状態に戻る
	if (attackTime_ >= kAttackDuration)
	{
		//クールタイムをリセットしてから接近状態に戻る
		boss->AttackCoolDown();

		auto nextState = std::make_shared<BossStateRun>(pBoss_, searchRadius_);
		boss->ChangeState(nextState);
		return;
	}
}

void BossStateAttack::Exit()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//攻撃コライダーの削除
	boss->RemoveAttackCollider();
}
