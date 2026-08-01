#include "BossStateRush.h"
#include "BossStateAttack.h"
#include "Boss.h"

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//突進の速度
	constexpr float kRushSpeed = 1000.0f;

	//突進の制限
	constexpr float kRushDuration = 0.8f;

	//突進の止まるプレイヤーとの距離感
	constexpr float kRushDistance = 50.0f;
}

BossStateRush::BossStateRush(std::weak_ptr<Boss> pBoss, float searchRadius):
	BossStateBase(pBoss,searchRadius),
	dashDir_(Vector3{0.0f,0.0f,0.0f}),
	dashTime_(0.0f)
{
}

void BossStateRush::Enter()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	dashTime_ = 0.0f;

	//突進方向を現在のプレイヤー位置から計算してそこに突進する感じ
	Vector3 toPlayer = boss->GetPlayerPos() - boss->GetPos();
	toPlayer.y_ = 0.0f;
	dashDir_ = toPlayer.Normalize();

	//突進アニメーションへ遷移
	boss->ChangeAnimation(AnimationState::BossRush);
}

void BossStateRush::Update()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	dashTime_ += kDeltaTime;

	//固定方向へ直進
	ApplyMove(boss, boss->GetPos(), dashDir_, kRushSpeed);
	//プレイヤーとの距離を計算
	Vector3 fromPlayer = boss->GetPos() - boss->GetPlayerPos();
	fromPlayer.y_ = 0.0f;
	float distance = fromPlayer.Length();
	//距離が縮まったか、時間切れになったらAttackへ遷移
	if (distance <= kRushDistance || dashTime_ >= kRushDuration)
	{
		auto nextState = std::make_shared<BossStateAttack>(pBoss_, searchRadius_);
		boss->ChangeState(nextState);
		return;
	}
}

void BossStateRush::Exit()
{
}
