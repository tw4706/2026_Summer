#include "TitlePlayer.h"

namespace
{
	constexpr float kFirstAngle = DX_PI_F;

	const Vector3 kFirstPos = { 200.0f, 0.0f, 0.0f };
	const Vector3 kFirstScale = { 2.0f, 2.0f, 2.0f };
	constexpr float kDeltaTime = 1.0f / 60.0f;

	const Vector3 kFirstMoveDir = { 0.0f, 0.0f, 0.0f };
	constexpr float kFirstMoveSpeed = 0.0f;
}

TitlePlayer::TitlePlayer():
	pos_(kFirstPos),
	scale_(kFirstScale),
	angle_(0.0f),
	moveDir_(kFirstMoveDir),
	moveSpeed_(kFirstMoveSpeed),
	isRunning_(false)
{
}

TitlePlayer::~TitlePlayer()
{
	model_.Release();
}

void TitlePlayer::Init()
{
	pos_ = kFirstPos;
	angle_ = kFirstAngle;

	//モデルのロード
	model_.Load(L"data/MV1/Player.mv1");

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());
	animation_.RegisterAnimName(AnimationState::Idle, L"Player|Idle");
	animation_.RegisterAnimName(AnimationState::Run, L"Player|Run");
	animation_.ChangeState(AnimationState::Idle);

	SetAngle(kFirstAngle);

	UpdateMatrix();
}

void TitlePlayer::Update()
{
	if (isRunning_)
	{
		pos_ += moveDir_ * moveSpeed_;
	}

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
	Matrix4x4 rotMat = Matrix4x4::RotateY(angle_+DX_PI_F);
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;

	model_.SetMatrix(worldMat);
}

void TitlePlayer::ChangeAnimation(AnimationState state)
{
	animation_.ChangeState(state);
}

void TitlePlayer::StartRun(const Vector3& dir, float speed)
{
	moveDir_ = dir;
	moveSpeed_ = speed;
	isRunning_ = true;
	ChangeAnimation(AnimationState::Run);
}

void TitlePlayer::StopRun()
{
	isRunning_ = false;
	moveSpeed_ = 0.0f;
	ChangeAnimation(AnimationState::Idle);
}

Vector3 TitlePlayer::GetForward() const
{
	return { sinf(angle_), 0.0f, cosf(angle_) };
}
