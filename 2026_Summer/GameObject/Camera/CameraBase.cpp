#include "CameraBase.h"

namespace 
{
    //カメラのNear・Far
    constexpr float kCameraNear = 200.0f;
    constexpr float kCameraFar = 1500.0f;

    //経過時間
    constexpr float kDeltaTime = 1.0f / 60.0f;

    //光の方向
    constexpr float kLightDirection = -1.8f;
}

CameraBase::CameraBase():
    pos_(0.0f, 0.0f, 0.0f),
    cameraTarget_(0.0f, 0.0f, 0.0f),
    fov_(DX_PI_F / 3.0f),
    fovTarget_(DX_PI_F / 3.0f),
    shakeTime_(0.0f),
    shakePower_(0.0f)
{
}

CameraBase::~CameraBase()
{
}

void CameraBase::Shake(float time, float power) {
    shakeTime_ = time;
    shakePower_ = power;
}

void CameraBase::StartZoom(float fov) {
    fovTarget_ = fov;
}

Vector3 CameraBase::UpdateShake() {
    if (shakeTime_ <= 0.0f) return Vector3(0, 0, 0);
    shakeTime_ -= kDeltaTime;

    float rx = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
    float ry = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
    float rz = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;

    return Vector3(rx * shakePower_, ry * shakePower_, rz * shakePower_);
}

void CameraBase::ApplyToDxLib() {
    //シェイクを加算した最終的な描画座標
    Vector3 cameraPos = pos_ + UpdateShake();

    //DxLibのカメラに座標と注視点を反映
    SetCameraPositionAndTarget_UpVecY(cameraPos.ToDxlibVector(), cameraTarget_.ToDxlibVector());
    SetupCamera_Perspective(fov_);
    SetCameraNearFar(kCameraNear, kCameraFar);

    //カメラの向きに合わせてライトの方向を調整
    Vector3 lightDir = (cameraTarget_ - cameraPos).Normalize();
    lightDir.y_ = kLightDirection;
    lightDir = lightDir.Normalize();
    SetLightDirection(lightDir.ToDxlibVector());
}
