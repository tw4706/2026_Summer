#include "CameraBase.h"
#include<Dxlib.h>

namespace 
{
    //カメラのNear・Far
    constexpr float kCameraNear = 100.0f;
    constexpr float kCameraFar = 5000.0f;

    //経過時間
    constexpr float kDeltaTime = 1.0f / 60.0f;

    //光の方向
    constexpr float kLightDirection = -1.8f;
}

CameraBase::CameraBase():
    pos_({ 0.0f, 0.0f, 0.0f }),
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

void CameraBase::Update(int stageModelHandle)
{
    //カメラの位置・注視点の計算
    UpdateRenderSystem();
}

void CameraBase::Shake(float time, float power) {
    shakeTime_ = time;
    shakePower_ = power;
}

void CameraBase::StartZoom(float fov) {
    fovTarget_ = fov;
}

Vector3 CameraBase::CheckCollCameraToStage(int stageModelHandle,const Vector3&startPos, const Vector3& endPos)
{
    //線分の始点と終点をDxLibのVECTORに変換
    VECTOR start = startPos.ToDxlibVector();
    VECTOR end = endPos.ToDxlibVector();

    //線分とポリゴンの当たり判定
    MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(stageModelHandle, -1, start, end);

    if (result.HitFlag == 1)
    {
        //当たった座標
        Vector3 hitPos = result.HitPosition;

        //ポリゴンの法線
        Vector3 normal = result.Normal;

        //押し出し処理
        hitPos += normal * 10.0f;

        //押し出した座標を返す
        return hitPos;
    }

    return end;
}

void CameraBase::SetCompulsoryPosAndTarget(Vector3& prevCameraPos, Vector3& prevCameraTarget)
{
    pos_ = prevCameraPos;
    cameraTarget_ = prevCameraTarget;
}

Vector3 CameraBase::UpdateShake()
{
    if (shakeTime_ <= 0.0f) return Vector3(0, 0, 0);
    shakeTime_ -= kDeltaTime;

    float rx = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
    float ry = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
    float rz = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;

    return Vector3(rx * shakePower_, ry * shakePower_, rz * shakePower_);
}

void CameraBase::UpdateRenderSystem() 
{
    //シェイクを加算した最終的な描画座標
    Vector3 cameraPos = pos_ + UpdateShake();

    //カメラに座標と注視点を反映
    SetCameraPositionAndTarget_UpVecY(cameraPos.ToDxlibVector(), cameraTarget_.ToDxlibVector());
    SetupCamera_Perspective(fov_);
    SetCameraNearFar(kCameraNear, kCameraFar);

    //カメラの向きに合わせてライトの方向を調整
    Vector3 lightDir = (cameraTarget_ - cameraPos).Normalize();
    lightDir.y_ = kLightDirection;
    lightDir = lightDir.Normalize();
    SetLightDirection(lightDir.ToDxlibVector());
}
