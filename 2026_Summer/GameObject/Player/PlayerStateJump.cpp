#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateAttack.h"
#include "PlayerStateJumpAttack.h"
#include "Player.h"
#include "Input.h"
#include "Camera/PlayerCamera.h"
#include "Math/Matrix4x4.h"
#include "SoundManager.h"

namespace
{
	//Jump
	const std::wstring_view kPlayerJump = L"Player|Jump";

	//Lerpの割合時間
	constexpr float kLerpRate = 0.25f;

	//回転の補間割合
	constexpr float kLerpAngleRate = 0.15f;

	//カメラの回転スピード
	constexpr float kCameraRotateSpeed = 0.03f;

	//向かう速度
	constexpr float kTagetVelocity = 8.0f;

	//地面と設置しているフレーム数
	constexpr int kGroundFrame = 2;

	//Run状態にせんいする閾値のスピード
	constexpr float kRunTransitionThereshouldSpeed = 1.5f;
}

PlayerStateJump::PlayerStateJump(std::weak_ptr<Player> pPlayer) :
	PlayerStateBase(pPlayer)
{
}

void PlayerStateJump::Enter()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	//アニメーションをジャンプに切り替える
	pPlayer->ChangeAnimation(AnimationState::Jump, kPlayerJump.data());

	if (pPlayer->GetIsGround())
	{
		Vector3 vel = pPlayer->GetVelocity();
		vel.y_ = pPlayer->jumpPower_;   //初速をセット
		pPlayer->SetVelocity(vel);

		//接地フラグを返す
		pPlayer->SetIsGround(false);
	}

	//ジャンプSEの再生
	SoundManager::GetInstance().PlaySe(SE::Jump);
}

void PlayerStateJump::Update()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	//アクティブなカメラを取得
	CameraBase* pCamera = GetActiveCamera();
	if (!pCamera) return;

	//空中での移動制御
	//スティック入力とキーボード入力を統合して取得
	Vector3 inputDir = Input::GetInstance().GetRawMoveInput();
	bool isKeyboardMoving = Input::GetInstance().HasMoveInput();

	Vector3 currentVel = pPlayer->GetVelocity();
	float currentAngle = pPlayer->moveAngle_;

	if (isKeyboardMoving)
	{
		float cameraYaw = pCamera->GetYaw();
		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		Vector3 playerDir = GetCameraLookMoveDirection();

		if (playerDir.LengthSq() > 0.001f)
		{
			Vector3 targetVel = playerDir * kTagetVelocity;
			currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, kLerpRate);
			currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, kLerpRate);

			//向きの変更
			float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
			float diff = playerAngle - currentAngle;
			while (diff > DX_PI_F) diff -= DX_PI_F * 2;
			while (diff < -DX_PI_F) diff += DX_PI_F * 2;

			currentAngle += diff * kLerpAngleRate;
		}
	}

	//重力・位置更新処理
	Vector3 pos = pPlayer->GetPos();
	currentVel.y_ -= pPlayer->GetGravity(); //重力をY軸に適用

	//現在のプレイヤーの速度を更新
	pPlayer->GetVelocity() = currentVel;

	//計算結果を反映
	pPlayer->SetVelocity(currentVel);
	pPlayer->moveAngle_ = currentAngle; //変更した向きを適用
	pPlayer->AddPosition();

	if (!pPlayer->IsLockOn())
	{
		//カメラ回転
		Vector3 stickR = Input::GetInstance().GetStickRight();
		pCamera->AddRotation(-stickR.x_ * kCameraRotateSpeed, -stickR.z_ * kCameraRotateSpeed);
	}

	//状態遷移判定
	if (pPlayer->GetIsGround())
	{
		landingFrameCount_++;
		//2フレーム以上連続で接地していたら着地確定
		if (landingFrameCount_ >= kGroundFrame)
		{
			//着地を確定した際、Yの速度をリセット
			Vector3 vel = pPlayer->GetVelocity();
			vel.y_ = 0.0f;
			pPlayer->SetVelocity(vel);

			float speedXZ = sqrtf(currentVel.x_ * currentVel.x_ + currentVel.z_ * currentVel.z_);
			if (speedXZ > kRunTransitionThereshouldSpeed)
			{
				pPlayer->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_));
			}
			else
			{
				pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_));
			}
			return;
		}
	}
	else
	{
		landingFrameCount_ = 0;  //接地が途切れた際にリセット
	}

	//攻撃ボタンが押されたら攻撃へ遷移
	if (Input::GetInstance().IsTriggered("attack"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateJumpAttack>(pPlayer_));
		return;
	}
}

void PlayerStateJump::Exit()
{
}
