#include "Oni.h"
#include "Katana.h"
#include "Matrix4x4.h"
#include"EnemyStateidle.h"
#include"CharacterStateBase.h"
#include "Collider/CapsuleCollider.h"
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

	//索敵範囲の半径
	const float kDebugSearchRadius = 500.0f;
}

Oni::Oni() :
	EnemyBase()
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
	hp_ = 50;
	isHit_ = false;

	//モデルのロード
	model_.Load(L"data/Oni.mv1");
	assert(model_.GetHandle() >= 0);

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());

	//コライダーの登録
	Vector3 colOffset = { 0.0f, 120.0f, 0.0f };
	this->CreateCollider<CapsuleCollider>(70.0f, 120.0f, colOffset);
}

void Oni::Update()
{
	Collidable::Update();

	if (!pCurrentState_)
	{
		//Characterのshared_ptrとして直接受け取る
		std::shared_ptr<Character> sharedSelf = shared_from_this();

		//弱参照にする
		std::weak_ptr<Character> weakSelf = sharedSelf;

		//EnemyStateIdle 側が Character の weak_ptr も受け取る
		auto sharedEnemy = std::dynamic_pointer_cast<Oni>(sharedSelf);
		std::weak_ptr<EnemyBase> weakEnemy = sharedEnemy;

		ChangeState(std::make_shared<EnemyStateIdle>(weakEnemy));
	}

	//ステートの更新
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
	Matrix4x4 scaleMat = Matrix4x4::Scale(kFirstScale.x_, kFirstScale.y_, kFirstScale.z_);
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_);
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;
	model_.SetMatrix(worldMat);

	//モデルの描画
	model_.Draw();

#ifdef _DEBUG
	DrawFormatString(200, 200, GetColor(255, 255, 255), L"Oni HP: %d", hp_);

	VECTOR center = VGet(pos_.x_, pos_.y_, pos_.z_);

	Vector3 playerPos = GetPlayerPos();
	Vector3 toPlayer = playerPos - pos_;

	float distSq = (toPlayer.x_ * toPlayer.x_) + (toPlayer.y_ * toPlayer.y_) + (toPlayer.z_ * toPlayer.z_);

	unsigned int color = GetColor(0, 255, 0);
	if (distSq <= kDebugSearchRadius * kDebugSearchRadius)
	{
		color = GetColor(255, 0, 0);
	}

	DrawSphere3D(center, kDebugSearchRadius, 8, color, GetColor(0, 0, 0), FALSE);

	//当たり判定の描画
	if (!colliders_.empty())
	{
		CapsuleCollider* pCap = static_cast<CapsuleCollider*>(colliders_[0].get());

		if (pCap)
		{
			VECTOR top = VGet(pCap->GetWorldB().x_, pCap->GetWorldB().y_, pCap->GetWorldB().z_);
			VECTOR bottom = VGet(pCap->GetWorldA().x_, pCap->GetWorldA().y_, pCap->GetWorldA().z_);

			//当たっていたら赤、通常は水色
			unsigned int lineColor = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 255);

			//描画
			DrawCapsule3D(top, bottom, pCap->GetRadius(), 8, lineColor, GetColor(0, 0, 0), FALSE);
		}
	}
#endif
}

void Oni::OnCollision(Collidable& coll)
{
	//相手の型が刀だった場合は何もしない
	if (dynamic_cast<Katana*>(&coll))
	{
		return;
	}

	//刀以外が当たった場合は普通にフラグを返す
	isHit_ = true;
}
