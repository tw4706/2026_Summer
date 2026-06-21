#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDodge.h"
#include "Player.h"
#include "Input.h"
#include "Camera/PlayerCamera.h"

namespace
{
	//プレイヤーアニメーション
	//Idle
	const wchar_t* kPlayerIdle = L"Player| Idle";

    //入力のしきい値
    constexpr float kInputEpsilon = 0.01f;

    //アナログスティックのデッドゾーン
    constexpr float kStickDeadZone = 0.15f;

    //移動時の線形補間
    constexpr float kMoveLerp = 0.15f;

    //カメラの回転速度
    constexpr float kCameraSpeed = 0.03f;
}

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<Player> pPlayer, Input& input, PlayerCamera& camera) :
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateIdle::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    pPlayer->ChangeAnimation(AnimationState::Idle, kPlayerIdle);

    //速度を0にする
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

void PlayerStateIdle::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

	DrawFormatString(0, 0, GetColor(255, 255, 255), L"IsGround: %d", pPlayer->GetIsGround());
	DrawFormatString(0, 20, GetColor(255, 255, 255), L"HasMoveInput: %d", input_.HasMoveInput());

	// カメラの回転
	Vector3 stickR = input_.GetStickRight();
	camera_.AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);

	// 空中にいるときかジャンプが押されたらJump状態へ遷移
	if (!pPlayer->GetIsGround() || input_.IsTriggered("jump"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
		return;
	}

	// 攻撃が押されたらAttack状態へ遷移
	if (input_.IsTriggered("attack"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, input_, camera_));
		return;
	}

	// 回避が押されたらDodge状態へ遷移
	if (input_.IsTriggered("dodge"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateDodge>(pPlayer_, input_, camera_));
		return;
	}

	// 移動入力があったらRun状態へ遷移
	if (input_.HasMoveInput())
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_, input_, camera_));
		return;
	}
}

void PlayerStateIdle::Exit()
{
}
