#include "TitlePlayer.h"

namespace
{
	const Vector3 kFirstPos = { 900.0f, 200.0f, -100.0f };
	const Vector3 kFirstScale = { 2.0f, 2.0f, 2.0f };
	constexpr float kDeltaTime = 1.0f / 60.0f;
}

TitlePlayer::TitlePlayer():
	pos_(kFirstPos),
	scale_(kFirstScale),
	angle_(0.0f)
{
}

TitlePlayer::~TitlePlayer()
{
	model_.Release();
}

void TitlePlayer::Init()
{
	pos_ = kFirstPos;
	angle_ = 0.0f;

	//モデルのロード
	model_.Load(L"data/MV1/Player.mv1");

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());
	animation_.RegisterAnimName(AnimationState::Idle, L"Player|Idle");
	animation_.ChangeState(AnimationState::Idle);

	SetAngle(DX_PI_F/1.5f);

	UpdateMatrix();
}

void TitlePlayer::Update()
{
	UpdateMatrix();
	animation_.Update(kDeltaTime);
}

void TitlePlayer::Draw()
{
	model_.Draw();
}

void TitlePlayer::UpdateMatrix()
{
	//行列を作成して見た目だけ反映
	Matrix4x4 scaleMat = Matrix4x4::Scale(scale_.x_, scale_.y_, scale_.z_);
	Matrix4x4 rotMat = Matrix4x4::RotateY(angle_);
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;

	model_.SetMatrix(worldMat);
}

void TitlePlayer::ChangeAnimation(AnimationState state)
{
	animation_.ChangeState(state);
}