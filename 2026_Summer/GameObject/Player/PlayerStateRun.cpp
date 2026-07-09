#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "PlayerStateDodge.h"
#include "PlayerStateGuard.h"
#include "Player.h"
#include "Input.h"
#include "Camera/PlayerCamera.h"
#include "Math/Matrix4x4.h"

namespace
{
	//Run
	const std::wstring_view kPlayerRun = L"Player|Run";

	//プレイヤーの移動速度
	constexpr float kSpeed = 10.0f;

	//停止する際の線形補間
	constexpr float kStopLerp = 0.2f;

	//カメラの回転速度
	constexpr float kCameraSpeed = 0.03f;

	//カメラのピッチ角
	constexpr float kCameraPitch = 0.3f;

	//スティックの回転の線形補間
	constexpr float kRotateLerpAnalogStick = 0.3f;

	//回転の線形補間
	constexpr float kRotateLerp = 0.3f;

	//移動の入力のしきい値
	constexpr float kRunEpsilon = 0.01f;

	//移動時の線形補間
	constexpr float kMoveLerp = 0.15f;

	//アナログスティックのデッドゾーン
	constexpr float kStickDeadZone = 0.15f;
}

PlayerStateRun::PlayerStateRun(std::weak_ptr<Player> pPlayer, CameraBase& camera) :
	PlayerStateBase(pPlayer, camera)
{
}

void PlayerStateRun::Enter()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	//Runアニメーションに遷移
	pPlayer->ChangeAnimation(AnimationState::Run, kPlayerRun.data());
}

void PlayerStateRun::Update()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	//プレイヤーの方向ベクトルを取得
	Vector3 playerDir = GetCameraLookMoveDirection();

	//現在の速度・向きの取得
	Vector3 currentVel = pPlayer->GetVelocity();
	float currentAngle = pPlayer->moveAngle_;

	//入力がある場合のみ移動・旋回を行う
	if (playerDir.LengthSq() > 0.001f)
	{
		//ターゲットの速度を計算して線形補間で滑らかに加速
		Vector3 targetVel = playerDir * kSpeed;
		currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, kMoveLerp);
		currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, kMoveLerp);

		//進行方向を向く
		float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
		float diff = playerAngle - currentAngle;
		while (diff > DX_PI_F)  diff -= DX_PI_F * 2.0f;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

		currentAngle += diff * kRotateLerp;
	}

	//計算した速度と角度をPlayerに適用し、移動させる
	pPlayer->SetVelocity(currentVel);
	pPlayer->moveAngle_ = currentAngle;
	pPlayer->AddPosition();

	//カメラの回転処理
	if (!pPlayer->IsLockOn())
	{
		Vector3 stickR = Input::GetInstance().GetStickRight();
		pCamera_.AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);
	}

	//ジャンプボタンが押されたらジャンプへ
	if (Input::GetInstance().IsTriggered("jump"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, pCamera_));
		return;
	}

	//空中にいる場合はジャンプ状態へ遷移
	if (!pPlayer->GetIsGround())
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, pCamera_));
		return;
	}

	//攻撃ボタンが押されたら攻撃へ遷移
	if (Input::GetInstance().IsTriggered("attack"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, pCamera_));
		return;
	}

	if (Input::GetInstance().IsTriggered("guard"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateGuard>(pPlayer_, pCamera_));
		return;
	}

	//回避ボタンが押されたら回避へ遷移
	if (Input::GetInstance().IsTriggered("dodge"))
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateDodge>(pPlayer_, pCamera_));
		return;
	}

	//移動入力が完全に無くなったらIdleへ遷移
	if (!Input::GetInstance().HasMoveInput())
	{
		pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, pCamera_));
		return;
	}
}

void PlayerStateRun::Exit()
{
}
