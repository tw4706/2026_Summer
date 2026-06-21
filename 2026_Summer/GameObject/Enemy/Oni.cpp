#include "Oni.h"
#include "Matrix4x4.h"
#include"CharacterStateBase.h"
#include"EnemyStateidle.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	//鬼の初期位置
	const Vector3 kFirstPos = { 200.0f, 0.0f, -100.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//初期回転角度
	const Vector3 kFirstRotate = { 0.0f, -DX_PI_F, 0.0f };

	//索敵半径
	const float kDebugSearchRadius = 200.0f; 
}

Oni::Oni() :
	EnemyBase(),
	moveAngle_(0.0f)
{
}

Oni::~Oni()
{
}

void Oni::Init()
{
	//初期化
	pos_ = kFirstPos;
	vel_ = { 0.0f, 0.0f, 0.0f };
	moveAngle_ = kFirstRotate.y_;

	//モデルのロード
	model_.Load(L"data/oni.mv1");
	assert(model_.GetHandle() >= 0);

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());
}

void Oni::Update()
{
	if (!pCurrentState_)
	{
		auto sharedEnemy = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());
		std::weak_ptr<EnemyBase> weakEnemy = sharedEnemy;

		pCurrentState_ = std::shared_ptr<EnemyStateIdle>(new EnemyStateIdle(weakEnemy));
		pCurrentState_->Enter();
	}

	if (pCurrentState_)
	{
		pCurrentState_->Update();
	}

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

#ifdef _DEBUG
	VECTOR center = VGet(pos_.x_, pos_.y_, pos_.z_);

	//プレイヤーの位置を取得
	Vector3 playerPos = GetPlayerPos();

	//敵からプレイヤーへのベクトルを計算
	Vector3 toPlayer = playerPos - pos_;

	//距離の二乗を計算
	float distSq = (toPlayer.x_ * toPlayer.x_) + (toPlayer.y_ * toPlayer.y_) + (toPlayer.z_ * toPlayer.z_);

	//範囲内なら赤色、範囲外なら緑色
	unsigned int color = GetColor(0, 255, 0);
	if (distSq <= kDebugSearchRadius * kDebugSearchRadius)
	{
		color = GetColor(255, 0, 0);
	}

	DrawSphere3D(center, kDebugSearchRadius, 8, color, GetColor(0, 0, 0), FALSE);
#endif

}
