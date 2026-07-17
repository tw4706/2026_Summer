#include "EnemyStateJump.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

namespace
{
	//経過時間
	const float kDeltaTime = 1.0f / 60.0f;

	//ジャンプにかかる時間
	const float kJumpDuration = 0.5f;

	//ジャンプの高さ
	const float kJumpHeight = 80.0f;
}

EnemyStateJump::EnemyStateJump(std::weak_ptr<EnemyBase> pEnemy, float searchRadius):
	EnemyStateBase(pEnemy,searchRadius),
	deltaTime_(0.0f)
{
}

void EnemyStateJump::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Jump);
}

void EnemyStateJump::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy) return;
}

void EnemyStateJump::Exit()
{
}
