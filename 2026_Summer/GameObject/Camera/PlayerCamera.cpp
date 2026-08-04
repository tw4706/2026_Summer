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

	//らーぷにかかる時間
	constexpr float kLerpRate = 0.5f;

	//ターゲットカメラの補間割合
	constexpr float kTargetCameraLerpRate = 0.3f;
}

PlayerCamera::PlayerCamera() :
	CameraBase()
{
}

PlayerCamera::~PlayerCamera()
{
}

void PlayerCamera::Init()
{
	if (!pPlayer_) return;

	//プレイヤーの注視点を取得
	cameraTarget_ = pPlayer_->GetCameraTarget();

	currentCameraTarget_ = Vector3::Lerp(currentCameraTarget_, cameraTarget_, kTargetCameraLerpRate);
	currentYaw_ = yaw_;
	currentPitch_ = pitch_;

	Matrix4x4 rot = Matrix4x4::RotateY(yaw_) * Matrix4x4::RotateX(pitch_);
	Vector3 offset = rot.TransformForVector(kTargetToCamera);
	pos_ = currentCameraTarget_ + offset;
}

void PlayerCamera::Update(int stageModelHandle)
{
	if (!pPlayer_) return;

	cameraTarget_ = pPlayer_->GetCameraTarget();

	//カメラの回転を線形補間で行う
	currentCameraTarget_ = cameraTarget_;
	currentYaw_ = Vector3::Lerp(currentYaw_, yaw_, kLerpRate);
	currentPitch_ = Vector3::Lerp(currentPitch_, pitch_, kLerpRate);

	//行列を用いて位置を計算
	Matrix4x4 rot = Matrix4x4::RotateY(currentYaw_) * Matrix4x4::RotateX(currentPitch_);
	Vector3 offset = rot.TransformForVector(kTargetToCamera);
	Vector3 targetCameraPos = currentCameraTarget_ + offset;
	pos_ = targetCameraPos;

	//注視点からカメラの座標に線分判定をする
	Vector3 nextCameraPos = CheckCollCameraToStage(stageModelHandle, cameraTarget_, targetCameraPos);

	pos_ = nextCameraPos;

	//基底クラスの更新処理
	CameraBase::Update(stageModelHandle);
}

void PlayerCamera::AddRotation(float yaw, float pitch)
{
	yaw_ += yaw;
	pitch_ += pitch;
	pitch_ = std::clamp(pitch_, -kLimitPitch, kLimitPitch);
}

void PlayerCamera::SetRotation(float yaw, float pitch)
{
	yaw_ = yaw;
	pitch_ = pitch;

	pitch_ = std::clamp(pitch_, -kLimitPitch, kLimitPitch);

	currentYaw_ = yaw_;
	currentPitch_ = pitch_;
}

void PlayerCamera::SetRotationToLockOn(const Vector3&lockOnPos, const Vector3& lockOnTarget)
{
	Vector3 cameraTarget = lockOnPos - lockOnTarget;

	//XZの距離の計算
	float length = sqrtf(cameraTarget.x_ * cameraTarget.x_ + cameraTarget.z_ * cameraTarget.z_);

	float currentYaw = 0.0f;
	float currentPitch = 0.0f;

	if (length > 0.0001f)
	{
		currentYaw = atan2f(cameraTarget.x_, cameraTarget.z_);
		currentPitch = atan2f(-cameraTarget.y_, length);
	}

	SetRotation(currentYaw, currentPitch);
}
