#include "Camera.h"
#include "Player/Player.h"
#include"Matrix4x4.h"
#include<DxLib.h>
#include<algorithm>

namespace
{
	//ターゲットからカメラの注視点
	const Vector3 kTargetToCamera = { 0.0f,250.0f,400.0f };

	//カメラのfov
	constexpr float kCameraFov = DX_PI_F / 3.0f;

	//カメラのfovのターゲット
	constexpr float kCameraFovTarget = DX_PI_F / 3.0f;

	//Near
	constexpr float kCameraNear = 200.0f;

	//Far
	constexpr float kCameraFar = 1500.0f;

	//DXライブラリの光の向き
	constexpr float kLightDirection = -1.8f;

	//真上真下まで行かないよう制限
	const float kLimitPitch = DX_PI_F / 4.0f; //60度

	//経過時間
	constexpr float kDeletaTime = 1.0f / 60.0f;
}

Camera::Camera() :
	GameObject(pos_, vel_,dir_),
	shakeTime_(0.0f),
	shakePower_(0.0f),
	fovTarget_(0.0f),
	fov_(0.0f),
	yaw_(0.0f),
	pitch_(0.0f),
	cameraTarget_(0.0f, 0.0f, 0.0f)
{
}

Camera::~Camera()
{

}

void Camera::Init()
{
	//視野角の初期化
	fov_ = kCameraFov;
	fovTarget_ = kCameraFovTarget;

	//注視点の設定
	cameraTarget_ = pPlayer_->GetCameraTarget();

	//カメラの配置座標(回転)
	Matrix4x4 rotY = Matrix4x4::RotateY(yaw_);
	Matrix4x4 rotX = Matrix4x4::RotateX(pitch_);
	Matrix4x4 rotMat = rotY * rotX;
	Vector3 offset = rotMat.TransformForVector(kTargetToCamera);

	pos_ = cameraTarget_ + offset;

	SetCameraPositionAndTarget_UpVecY(pos_.ToDxlibVector(), cameraTarget_.ToDxlibVector());

	SetupCamera_Perspective(kCameraFov);
	SetCameraNearFar(kCameraNear, kCameraFar);
}

void Camera::Update()
{
	//プレイヤーがいないときは更新しない
	if (!pPlayer_)return;

	UpdateCamera();
}

void Camera::UpdateCamera()
{
	//カメラの注視点
	cameraTarget_ = pPlayer_->GetCameraTarget();

	////ズームの補間
	//fov_ = Vector3::Lerp(fov_, fovTarget_, 0.07f);
	////ズーム後少しずつ元に戻す
	//fovTarget_ = Vector3::Lerp(fovTarget_, kCameraFovTarget, 0.1f);
	//SetupCamera_Perspective(fov_);

	//回転
	Matrix4x4 rotY = Matrix4x4::RotateY(yaw_);
	Matrix4x4 rotX = Matrix4x4::RotateX(pitch_);
	Matrix4x4 rotMat = rotY * rotX;
	Vector3 offset = rotMat.TransformForVector(kTargetToCamera);

	//カメラの位置
	Vector3 cameraPos = cameraTarget_ + offset;
	cameraPos.y_ = (std::max)(cameraPos.y_, 50.0f);

	//カメラの補間
	pos_ = Vector3::Lerp(pos_, cameraPos, 0.5f);

	//カメラのシェイク適用
	pos_ += UpdateShake();

	//光の当たる方向をカメラの向きに合わせて暗いところがないようにする
	Vector3 lightDir = (cameraTarget_ - pos_).Normalize();

	//少し上から照らす
	lightDir.y_ = kLightDirection;

	//正規化
	lightDir = lightDir.Normalize();
	SetLightDirection(lightDir.ToDxlibVector());

	SetCameraPositionAndTarget_UpVecY(pos_.ToDxlibVector(), cameraTarget_.ToDxlibVector());
}

void Camera::AddRotation(float yaw, float pitch)
{
	yaw_ += yaw;
	pitch_ += pitch;

	pitch_ = std::clamp(pitch_, -kLimitPitch, kLimitPitch);
}

//シェイク
void Camera::Shake(float time, float power)
{
	shakeTime_ = time;
	shakePower_ = power;
}

void Camera::StartZoom(float fov)
{
	fovTarget_ = fov;
}

Vector3 Camera::UpdateShake()
{
	if (shakeTime_ <= 0.0f)
	{
		return Vector3(0, 0, 0);
	}

	shakeTime_ -= kDeletaTime;

	//r=(rand/RAND_MAX-0.5f)*2.0f;の計算は-1.0fから1.0fの範囲でランダムな値を生成
	float rx = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
	float ry = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
	float rz = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;

	Vector3 shake = { rx * shakePower_,ry * shakePower_,rz * shakePower_ };

	//正規化
	shake.Normalize();

	return shake;
}
