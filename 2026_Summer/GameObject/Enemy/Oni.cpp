#include "Oni.h"
#include "Matrix4x4.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	//敵アニメーション
	//Idle
	const wchar_t* kOniIdle = L"Armature|Idle";

	//鬼の初期位置
	const Vector3 kFirstPos = { 200.0f, 0.0f, -100.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//初期回転角度
	const Vector3 kFirstRotate = { 0.0f, -DX_PI_F, 0.0f };
}

Oni::Oni() :
	EnemyBase(),
	moveAngle_(0.0f)
{
}

Oni::~Oni()
{
	Character::~Character();
}

void Oni::Init()
{
	//初期化
	pos_ = kFirstPos;
	vel_ = { 0.0f, 0.0f, 0.0f };
	moveAngle_ = -DX_PI_F;

	//モデルのロード
	model_.Load(L"data/oni.mv1");
	assert(model_.GetHandle() >= 0);

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());
	animation_.ChangeState(AnimationState::Idle, kOniIdle);
}

void Oni::Update()
{
	//アニメーションの更新
	animation_.Update(1.0f / 60.0f);

	//拡縮行列
	Matrix4x4 scaleMat = Matrix4x4::Scale(kFirstScale.x_, kFirstScale.y_, kFirstScale.z_);
	//回転行列
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_);
	//移動行列
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	//行列の合成
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;

	//モデルに行列をセット
	model_.SetMatrix(worldMat);
}

void Oni::Draw()
{
	//モデルの描画
	model_.Draw();
}
