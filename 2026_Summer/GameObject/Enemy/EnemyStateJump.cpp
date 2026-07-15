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

	enemy->ChangeAnimation(AnimationState::Run);

	//ジャンプ中は経路追従に干渉されたくないのでクリア
	if (enemy->pathFollower_.HasPath())
	{
		enemy->pathFollower_.ClearPath();
	}

	jumpStartPos_ = enemy->GetPos();
	jumpEndPos_ = enemy->jumpTargetPos_;
	deltaTime_ = 0.0f;

	//進行方向を先に向けておく
	Vector3 toTarget = jumpEndPos_ - jumpStartPos_;
	toTarget.y_ = 0.0f;
	if (toTarget.Length() > 0.0001f)
	{
		toTarget.Normalize();
		enemy->moveAngle_ = std::atan2f(toTarget.x_, -toTarget.z_);
	}
}

void EnemyStateJump::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy) return;

	deltaTime_ += kDeltaTime;
	float t = deltaTime_ / kJumpDuration;

	if (t >= 1.0f)
	{
		//着地位置を確定させてIdleへ戻る
		enemy->SetPos(jumpEndPos_);

		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	//水平方向は線形補間
	Vector3 pos = jumpStartPos_ + (jumpEndPos_ - jumpStartPos_) * t;

	//縦方向は放物線(0→1→0の山なり)を高低差の線形補間に加算
	float baseY = jumpStartPos_.y_ + (jumpEndPos_.y_ - jumpStartPos_.y_) * t;
	float arc = kJumpHeight * 4.0f * t * (1.0f - t);
	pos.y_ = baseY + arc;

	enemy->SetPos(pos);
}

void EnemyStateJump::Exit()
{
}
