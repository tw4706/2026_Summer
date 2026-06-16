#include "PlayerCamera.h"
#include "Player/Player.h"
#include "Matrix4x4.h"
#include"Input.h"
#include <DxLib.h>
#include <algorithm>

namespace
{
    //ターゲットからのカメラの座標
    const Vector3 kTargetToCamera = { 0.0f, 250.0f, 400.0f };

    //限界の角度
    const float kLimitPitch = DX_PI_F / 4.0f;
}

PlayerCamera::PlayerCamera():
    CameraBase(),
    yaw_(0.0f),
    pitch_(0.0f)
{
}

PlayerCamera::~PlayerCamera()
{
}

void PlayerCamera::Init()
{
    if (!pPlayer_) return;

    cameraTarget_ = pPlayer_->GetCameraTarget();

    Matrix4x4 rot = Matrix4x4::RotateY(yaw_) * Matrix4x4::RotateX(pitch_);
    Vector3 offset = rot.TransformForVector(kTargetToCamera);
    pos_ = cameraTarget_ + offset;

    ApplyToDxLib();
}

void PlayerCamera::Update(Input&input)
{
    if (!pPlayer_) return;

    // プレイヤーの注視点を取得
    cameraTarget_ = pPlayer_->GetCameraTarget();

    //右スティックの取得
    Vector3 stickR = input.GetStickRight();

    AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);

    cameraTarget_ = pPlayer_->GetCameraTarget();

    // 回転から位置を計算
    Matrix4x4 rot = Matrix4x4::RotateY(yaw_) * Matrix4x4::RotateX(pitch_);
    Vector3 offset = rot.TransformForVector(kTargetToCamera);
    Vector3 targetCamPos = cameraTarget_ + offset;

    // 地面にめり込まないようにY座標を制限
    targetCamPos.y_ = (std::max)(targetCamPos.y_, 50.0f);

    // カメラの滑らかな移動（補間）
    pos_ = Vector3::Lerp(pos_, targetCamPos, 0.5f);

    // DxLibに反映
    ApplyToDxLib();
}

void PlayerCamera::AddRotation(float yaw, float pitch)
{
    yaw_ += yaw;
    pitch_ += pitch;
    pitch_ = std::clamp(pitch_, -kLimitPitch, kLimitPitch);
}
