#include "EnemyStateJump.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

namespace
{
	//ジャンプの初速
	const float kJumpPower = 10.0f;

	//重力
	const float kGravity = 0.6f;

	//ジャンプ中の進む速度
	const float kJumpMoveSpeed = 2.0f;
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

	velocityY_ = kJumpPower;

	//着地地点の目標となるベクトルの計算
	Vector3 toTarget = jumpEndPos_ - jumpStartPos_;
	toTarget.y_ = 0.0f;
	moveDir_ = toTarget.Normalize();
}

void EnemyStateJump::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy) return;

	Vector3 currentPos = enemy->GetPos();

	currentPos = currentPos + moveDir_ * kJumpMoveSpeed;
	currentPos.y_ += velocityY_;
	velocityY_ -= kGravity;

	if (velocityY_ < 0.0f && currentPos.y_ <= jumpEndPos_.y_)
	{
		//高さを着地地点に合わせる
		currentPos.y_ = jumpEndPos_.y_;
		enemy->SetPos(currentPos);

		//ジャンプが終了した後Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	enemy->SetPos(currentPos);
}

void EnemyStateJump::Exit()
{
}

void EnemyStateJump::SetUpJumpPos(const Vector3& startPos, const Vector3& targetPos)
{
	jumpStartPos_ = startPos;
	jumpEndPos_ = targetPos;
}
