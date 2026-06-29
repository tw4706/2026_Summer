#include "BigMan.h"
#include "Katana.h"
#include "Matrix4x4.h"
#include"EnemyStateidle.h"
#include"CharacterStateBase.h"
#include "Collider/CapsuleCollider.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	//‹S‚Ì‰ŠúˆÊ’u
	const Vector3 kFirstPos = { 200.0f, 0.0f, -1000.0f };

	//‰ŠúƒXƒP[ƒ‹
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//‰Šú‰ñ“]Šp“x
	const Vector3 kFirstRotate = { 0.0f, -DX_PI_F, 0.0f };

	//õ“G”ÍˆÍ‚Ì”¼Œa
	const float kDebugSearchRadius = 500.0f;
}

BigMan::BigMan()
{
}

void BigMan::Init()
{
	//‰Šú‰»
	pos_ = kFirstPos;
	moveAngle_ = kFirstRotate.y_;
	hp_ = 50;

	//ƒ‚ƒfƒ‹‚Ìƒ[ƒh
	model_.Load(L"data/MV1/BigMan.mv1");
	assert(model_.GetHandle() >= 0);

	//ƒRƒ‰ƒCƒ_[‚Ì“o˜^
	Vector3 colOffset = { 0.0f, 120.0f, 0.0f };
	this->CreateCollider<CapsuleCollider>(70.0f, 120.0f, colOffset);

	EnemyBase::Init();
}

void BigMan::Draw()
{
	Matrix4x4 scaleMat = Matrix4x4::Scale(kFirstScale.x_, kFirstScale.y_, kFirstScale.z_);
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_);
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;
	model_.SetMatrix(worldMat);

	//ƒ‚ƒfƒ‹‚Ì•`‰æ
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

	//“–‚½‚è”»’è‚Ì•`‰æ
	if (!colliders_.empty())
	{
		CapsuleCollider* pCap = static_cast<CapsuleCollider*>(colliders_[0].get());

		if (pCap)
		{
			VECTOR top = VGet(pCap->GetWorldB().x_, pCap->GetWorldB().y_, pCap->GetWorldB().z_);
			VECTOR bottom = VGet(pCap->GetWorldA().x_, pCap->GetWorldA().y_, pCap->GetWorldA().z_);

			//“–‚½‚Á‚Ä‚¢‚½‚çÔA’Êí‚Í…F
			unsigned int lineColor = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 255);

			//•`‰æ
			DrawCapsule3D(top, bottom, pCap->GetRadius(), 8, lineColor, GetColor(0, 0, 0), FALSE);
		}
	}
#endif
}
