#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateAttack.h"
#include "PlayerStateJumpAttack.h"
#include "Player.h"
#include "Input.h"
#include "Camera/PlayerCamera.h"
#include "Math/Matrix4x4.h"

namespace
{
	//Jump
	const std::wstring_view kPlayerJump = L"Player|Jump";
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
			Vector3 targetVel = playerDir * 8.0f;
			currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, 0.25f);
			currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, 0.25f);

			//向きの変更
			float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
			float diff = playerAngle - currentAngle;
			while (diff > DX_PI_F) diff -= DX_PI_F * 2;
			while (diff < -DX_PI_F) diff += DX_PI_F * 2;

			currentAngle += diff * 0.15f;
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
		pCamera->AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);
	}

	//状態遷移判定
	if (pPlayer->GetIsGround())
	{
		landingFrameCount_++;
		//2フレーム以上連続で接地していたら着地確定
		if (landingFrameCount_ >= 2)
		{
			//着地を確定した際、Yの速度をリセット
			Vector3 vel = pPlayer->GetVelocity();
			vel.y_ = 0.0f;
			pPlayer->SetVelocity(vel);

			float speedXZ = sqrtf(currentVel.x_ * currentVel.x_ + currentVel.z_ * currentVel.z_);
			if (speedXZ > 1.5f)
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
