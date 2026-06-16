#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDodge.h"
#include "Player.h"
#include "Input.h"
#include "Camera/CameraBase.h"

namespace
{
    //入力のしきい値
    constexpr float kInputEpsilon = 0.01f;

    //アナログスティックのデッドゾーン
    constexpr float kStickDeadZone = 0.15f;

    //移動時の線形補間
    constexpr float kMoveLerp = 0.15f;

    //カメラの回転速度
    constexpr float kCameraSpeed = 0.03f;
}

PlayerStateIdle::PlayerStateIdle(Player* pPlayer, Input& input, CameraBase& camera) :
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateIdle::Enter()
{
    if (!pPlayer_) return;

	//Idle状態に遷移
    pPlayer_->ChangeAnimation(AnimationState::Idle);

    //速度を0にする
    pPlayer_->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

void PlayerStateIdle::Update()
{;
    if (!pPlayer_) return;

	DrawFormatString(0, 0, GetColor(255, 255, 255), "IsGround: %d", pPlayer_->GetIsGround());
	DrawFormatString(0, 20, GetColor(255, 255, 255), "HasMoveInput: %d", input_.HasMoveInput());

	// カメラの回転
	Vector3 stickR = input_.GetStickRight();
	camera_.AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);

	// 空中にいるときかジャンプが押されたらJump状態へ遷移
	if (!pPlayer_->GetIsGround() || input_.IsTriggered("jump"))
	{
		pPlayer_->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
		return;
	}

	// 攻撃が押されたらAttack状態へ遷移
	if (input_.IsTriggered("attack"))
	{
		pPlayer_->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, input_, camera_));
		return;
	}

	// 回避が押されたらDodge状態へ遷移
	//if (input_.IsTriggered("dodge"))
	//{
	//	pPlayer->ChangeState(std::make_shared<PlayerStateDodge>(pPlayer_, input_, camera_));
	//	return;
	//}

	// 移動入力があったらRun状態へ遷移
	if (input_.HasMoveInput())
	{
		pPlayer_->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_, input_, camera_));
		return;
	}
}

void PlayerStateIdle::Exit()
{
}
