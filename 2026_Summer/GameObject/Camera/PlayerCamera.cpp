#include "PlayerCamera.h"
#include "Player/Player.h"
#include "Math/Matrix4x4.h"
#include <DxLib.h>
#include <algorithm>

namespace
{
	//ターゲットからのカメラの座標
	const Vector3 kTargetToCamera = { 0.0f, 60.0f,380.0f };

	//限界の角度
	const float kLimitPitch = DX_PI_F / 4.0f;
}

PlayerCamera::PlayerCamera() :
	CameraBase(),
	yaw_(0.0f),
	pitch_(0.0f),
	currentYaw_(0.0f),
	currentPitch_(0.0f)
{
}

PlayerCamera::~PlayerCamera()
{
}

void PlayerCamera::Init()
{
	if (!pPlayer_) return;

	cameraTarget_ = pPlayer_->GetCameraTarget();

	currentYaw_ = yaw_;
	currentPitch_ = pitch_;

	Matrix4x4 rot = Matrix4x4::RotateY(yaw_) * Matrix4x4::RotateX(pitch_);
	Vector3 offset = rot.TransformForVector(kTargetToCamera);
	pos_ = cameraTarget_ + offset;
}

void PlayerCamera::Update(int stageModelHandle)
{
	if (!pPlayer_) return;

	//プレイヤーの注視点を取得
	cameraTarget_ = pPlayer_->GetCameraTarget();

	//カメラの回転を線形補間
	currentYaw_ = currentYaw_ + (yaw_ - currentYaw_) * 0.5f;
	currentPitch_ = currentPitch_ + (pitch_ - currentPitch_) * 0.5f;

	//行列を用いて位置を計算
	Matrix4x4 rot = Matrix4x4::RotateY(currentYaw_) * Matrix4x4::RotateX(currentPitch_);
	Vector3 offset = rot.TransformForVector(kTargetToCamera);
	Vector3 targetCamPos = cameraTarget_ + offset;
	pos_ = targetCamPos;

	////注視点から次のカメラの座標に線分判定を行う
	Vector3 finalCameraPos = CheckCollCameraToStage(stageModelHandle, cameraTarget_, targetCamPos);

	pos_ = finalCameraPos;

	//親クラスの更新処理
	CameraBase::Update(stageModelHandle);
}

void PlayerCamera::AddRotation(float yaw, float pitch)
{
	yaw_ += yaw;
	pitch_ += pitch;
	pitch_ = std::clamp(pitch_, -kLimitPitch, kLimitPitch);
}
