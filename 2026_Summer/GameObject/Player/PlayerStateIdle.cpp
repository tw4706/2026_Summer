#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDodge.h"
#include "PlayerStateGuard.h"
#include "Player.h"
#include "Input.h"
#include "Camera/PlayerCamera.h"

namespace
{
	//プレイヤーアニメーション
	//Idle
	const std::wstring_view kPlayerIdle = L"Player|Idle";

    //入力のしきい値
    constexpr float kInputEpsilon = 0.01f;

    //移動時の線形補間
    constexpr float kMoveLerp = 0.15f;

    //カメラの回転速度
    constexpr float kCameraSpeed = 0.03f;
}

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<Player> pPlayer) :
	PlayerStateBase(pPlayer)
{
}

void PlayerStateIdle::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    pPlayer->ChangeAnimation(AnimationState::Idle, kPlayerIdle.data());

    //速度を0にする
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });

	//無敵をオフにする
	pPlayer->isInvincible_ = false;
}

void PlayerStateIdle::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

	//アクティブなカメラを取得
	CameraBase* pCamera = GetActiveCamera();
	if (!pCamera) return;

	//ロックオンをしていない場合は通常のカメラの回転にする
	if (!pPlayer->IsLockOn())
	{
		//カメラの回転
		Vector3 stickR = Input::GetInstance().GetStickRight();
		pCamera->AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);
	}

	// 空中にいるときかジャンプが押されたらJump状態へ遷移
	if (!pPlayer->GetIsGround() || Input::GetInstance().IsTriggered("jump"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_));
		return;
	}

	// 攻撃が押されたらAttack状態へ遷移
	if (Input::GetInstance().IsTriggered("attack"))
	{
		pPlayer->GetComboManager().ResetCombo();

		pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_));
		return;
	}
	if (Input::GetInstance().IsTriggered("guard"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateGuard>(pPlayer_));
		return;
	}

	// 回避が押されたらDodge状態へ遷移
	if (Input::GetInstance().IsTriggered("dodge"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateDodge>(pPlayer_));
		return;
	}

	// 移動入力があったらRun状態へ遷移
	if (Input::GetInstance().HasMoveInput())
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_));
		return;
	}
}

void PlayerStateIdle::Exit()
{
}
