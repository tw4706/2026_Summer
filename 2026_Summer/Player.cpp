#include "Player.h"
#include"Input.h"
#include"Camera.h"
#include"Matrix4x4.h"
#include<Dxlib.h>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f, 30.0f, 0.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//プレイヤーの移動速度
	constexpr float kSpeed = 10.0f;

	//入力のしきい値
	constexpr float kInputEpsilon = 0.01f;

	//アナログスティックのデッドゾーン
	constexpr float kStickDeadZone = 0.04f;

	//停止する際の線形補間
	constexpr float kStopLerp = 0.2f;

	//カメラの回転速度
	constexpr float kCameraSpeed = 0.03f;
	constexpr float kCameraPitch = 0.3f;

	constexpr float kRotateLerpAnalogStick = 0.3f;

	//移動時の線形補間
	constexpr float kMoveLerp = 0.15f;
}

Player::Player() :
	GameObject(pos_, vel_, dir_),
	moveAngle_(0.0f),
	model_(-1)
{
}

Player::~Player()
{

}

void Player::Init()
{
	pos_ = kFirstPos;

	//モデルのロード
	model_ = MV1LoadModel("data/Player.mv1");

	//モデルの位置のセット
	MV1SetPosition(model_, kFirstPos.ToDxlibVector());

	//モデルの拡大率
	MV1SetScale(model_, kFirstScale.ToDxlibVector());
}

void Player::Update(Input&input)
{
	//移動
	Move(input);

	//位置の更新
	MV1SetPosition(model_, pos_.ToDxlibVector());

	//回転
	MV1SetRotationXYZ(model_, VGet(0.0f, -moveAngle_, 0.0f));
}

void Player::Draw()
{
	//モデルの描画
	MV1DrawModel(model_);
}

Vector3 Player::GetCameraTarget() const
{
	return pos_;
}

void Player::Move(Input& input)
{
	//移動ベクトルの初期化
	vel_ = { 0.0f, 0.0f, 0.0f };

	//キーボード入力から方向作る 
	Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
	if (input.IsPressed("up")) inputDir.z_ += 1.0f;
	if (input.IsPressed("down")) inputDir.z_ -= 1.0f;
	if (input.IsPressed("left")) inputDir.x_ -= 1.0f;
	if (input.IsPressed("right")) inputDir.x_ += 1.0f;

	//入力があるときだけ 
	if (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > kInputEpsilon)
	{
		//カメラの角度を基準にして移動方向を回転させる
		float cameraYaw = 0.0f;
		if (pCamera_)
		{
			cameraYaw = pCamera_->GetYaw();
		}

		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		Vector3 playerDir = rotMat.TransformForVector(inputDir);

		//正規化 
		playerDir.Normalize();

		//移動 
		vel_.x_ = playerDir.x_ * kSpeed;
		vel_.z_ = playerDir.z_ * kSpeed;

		//向き更新 
		float playerAngle = atan2f(-playerDir.x_, playerDir.z_);
		float diff = playerAngle - moveAngle_;

		//キャラクターの角度を補間する
		while (diff > DX_PI_F) diff -= DX_PI_F * 2;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2;

		//スムーズに回転させる
		moveAngle_ += diff;
	}

	//アナログスティックの更新 
	UpdateAnalogStick(input);

	//位置の反映
	pos_ += vel_;
}

void Player::UpdateAnalogStick(Input& input)
{
	//左アナログスティックの取得
	Vector3 stickL = input.GetStickLeft();
	Vector3 stickR = input.GetStickRight();

	Vector3 playerDir = { 0.0f, 0.0f, 0.0f };

	if (stickL.LengthSq() > kStickDeadZone)
	{
		float cameraYaw = pCamera_ ? pCamera_->GetYaw() : 0.0f;

		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		playerDir = rotMat.TransformForVector(stickL).Normalize();

		//速度の線形補間
		Vector3 targetVel = playerDir * kSpeed;
		vel_.x_ = Vector3::Lerp(vel_.x_, targetVel.x_, kMoveLerp);
		vel_.z_ = Vector3::Lerp(vel_.z_, targetVel.z_, kMoveLerp);

		//プレイヤーの向いている方向の更新
		float playerAngle = atan2f(-playerDir.x_, playerDir.z_);
		float diff = playerAngle - moveAngle_;

		while (diff > DX_PI_F) diff -= DX_PI_F * 2.0f;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

		moveAngle_ += diff * kRotateLerpAnalogStick;
	}
	else
	{
		//入力がない場合は減速させる
		vel_.x_ = Vector3::Lerp(vel_.x_, 0.0f, kStopLerp);
		vel_.z_ = Vector3::Lerp(vel_.z_, 0.0f, kStopLerp);
	}

	//カメラ回転
	if (pCamera_)
	{
		pCamera_->AddRotation(stickR.x_ * kCameraSpeed, stickR.z_ * kCameraSpeed);
	}
}