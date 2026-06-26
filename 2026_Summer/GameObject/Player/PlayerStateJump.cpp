#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "Player.h"
#include "Input.h"
#include "Camera/PlayerCamera.h"
#include "Matrix4x4.h"

namespace
{
	//Jump
	const wchar_t* kPlayerJump = L"Player|Jump";
}

PlayerStateJump::PlayerStateJump(std::weak_ptr<Player> pPlayer, Input& input, PlayerCamera& camera) :
	PlayerStateBase(pPlayer, input, camera)
{
}

void PlayerStateJump::Enter()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	// アニメーションをジャンプに切り替える
	pPlayer->ChangeAnimation(AnimationState::Jump, kPlayerJump);

	if (pPlayer->GetIsGround())
	{
		Vector3 vel = pPlayer->GetVelocity();
		vel.y_ = pPlayer->GetJumpPower();   //初速をセット
		pPlayer->SetVelocity(vel);

		//接地フラグを返す
		pPlayer->SetIsGround(false);
	}
}

void PlayerStateJump::Update()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	//空中での旋回・移動制御
	Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
	if (input_.IsPressed("up"))    inputDir.z_ += 1.0f;
	if (input_.IsPressed("down"))  inputDir.z_ -= 1.0f;
	if (input_.IsPressed("left"))  inputDir.x_ -= 1.0f;
	if (input_.IsPressed("right")) inputDir.x_ += 1.0f;

	bool isKeyboardMoving = (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > 0.01f);
	Vector3 currentVel = pPlayer->GetVelocity();
	float currentAngle = pPlayer->GetMoveAngle();

	if (isKeyboardMoving)
	{
		float cameraYaw = camera_.GetYaw();
		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		Vector3 playerDir = rotMat.TransformForVector(-inputDir).Normalize();

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

	//重力・位置更新処理
	Vector3 pos = pPlayer->GetPos();
	currentVel.y_ -= pPlayer->GetGravity(); //重力をY軸に適用

	//地面との接地判定
	//if (pos.y_ + currentVel.y_ <= 0.0f)
	//{
	//	pos.y_ = 0.0f;
	//	currentVel.y_ = 0.0f;
	//	pPlayer->SetIsGround(true);
	//}

	//現在のプレイヤーの速度を更新
	pPlayer->GetVelocity() = currentVel;

	//計算結果を反映
	pPlayer->SetVelocity(currentVel);
	pPlayer->SetMoveAngle(currentAngle); //変更した向きを適用
	pPlayer->AddPosition();

	//カメラ回転
	Vector3 stickR = input_.GetStickRight();
	camera_.AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);

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
				pPlayer->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_, input_, camera_));
			}
			else
			{
				pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
			}
			return;
		}
	}
	else
	{
		landingFrameCount_ = 0;  //接地が途切れた際にリセット
	}
}

void PlayerStateJump::Exit()
{
}
