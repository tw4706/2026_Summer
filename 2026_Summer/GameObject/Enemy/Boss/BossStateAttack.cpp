#include "BossStateAttack.h"
#include "BossStateRun.h"
#include "Boss.h"
#include "EnemyAttackDataLoader.h"
#include "SoundManager.h"

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//攻撃判定の半径
	constexpr float kAttackRadius = 200.0f;

	//攻撃判定を出す距離
	constexpr float kAttackDistance = 150.0f;

	//攻撃モーション全体の時間
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

	//ボスの攻撃力を取得して攻撃コライダーに設定する
	int damage = 0;
	if (const EnemyAttackDataLoader* loader = boss->GetAttackDataLoader())
	{
		if (const AttackData* data = loader->GetAttackData(AttackType::NormalAttack))
		{
			damage = static_cast<int>(data->damage_);
		}
	}

	//攻撃コライダーを生成
	boss->CreateAttackCollider(kAttackRadius, kAttackDistance, damage);

	//ボスの攻撃のSE再生
	SoundManager::GetInstance().PlaySe(SE::BossAttack);
}

void BossStateAttack::Update()
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

void BossStateAttack::Exit()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//攻撃コライダーの削除
	boss->RemoveAttackCollider();
}
