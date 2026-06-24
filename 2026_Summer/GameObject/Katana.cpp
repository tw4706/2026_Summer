#include "Katana.h"
#include "Animation.h"
#include "Collider/CapsuleCollider.h"
#include<memory>

namespace
{
	///刀のモデル関連
	//刀の拡大率
	const Vector3 kKatanaScale = { 2.0f, 2.0f, 2.0f };

	//刀の回転
	const Vector3 kKatanaRotate = { 0.0f, -DX_PI_F / 2.0f, DX_PI_F / 2.0f };

	//刀の移動
	const Vector3 kKatanaTransform = { 20.0f, 10.0f, 0.0f };
}

Katana::Katana(Vector3 pos, Vector3 vel, float dir) :
	Collidable(pos, vel, dir)
{
}

Katana::~Katana()
{
}

void Katana::Init()
{
	//刀モデルのロード
	katanaModel_.Load(L"data/Tachi.mv1");

	auto pCapsule = std::make_unique<CapsuleCollider>(15.0f, 80.0f, Vector3{ 0.0f, 0.0f, 0.0f });
	this->AddCollider(std::move(pCapsule));
}

void Katana::Update(const MATRIX& handMat, AnimationState ownerState)
{
	Collidable::Update();

	Vector3 katanaScale = kKatanaScale;
	Vector3 katanaRotate = kKatanaRotate;
	Vector3 katanaTransform = kKatanaTransform;

	if (ownerState == AnimationState::Run)
	{
		katanaRotate.x_ += DX_PI_F / 4.0f;
		katanaRotate.z_ += DX_PI_F / 3.0f;
	}

	// 行列計算（今までPlayer::Drawにあったロジックをそのまま移動）
	Matrix4x4 scaleMat = Matrix4x4::Scale(katanaScale.x_, katanaScale.y_, katanaScale.z_);
	Matrix4x4 rotMat = Matrix4x4::RotateX(katanaRotate.x_) * Matrix4x4::RotateY(katanaRotate.y_) * Matrix4x4::RotateZ(katanaRotate.z_);
	Matrix4x4 transMat = Matrix4x4::Translate(katanaTransform.x_, katanaTransform.y_, katanaTransform.z_);
	Matrix4x4 mat = scaleMat * rotMat * transMat;

	worldMat_ = MMult(mat.ToDxLibMatrix(), handMat);
	katanaModel_.SetMatrix(worldMat_);

	//当たり判定の更新：worldMat_を使って自分のpos_を刀の根本位置に同期させる
	//ローカル座標での始点と終点
	VECTOR localStart = VGet(0.0f, -30.0f, 0.0f);
	VECTOR localEnd = VGet(0.0f, 70.0f, 0.0f);

	//ワールド座標での始点と終点
	VECTOR worldStart = VTransform(localStart, worldMat_);
	VECTOR worldEnd = VTransform(localEnd, worldMat_);

	if (!colliders_.empty())
	{
		CapsuleCollider* pCapsule = static_cast<CapsuleCollider*>(colliders_[0].get());
		if (pCapsule)
		{
			pCapsule->SetWorldPos(Vector3{ worldStart.x, worldStart.y, worldStart.z },
				Vector3{ worldEnd.x,  worldEnd.y, worldEnd.z });
		}
	}
}

void Katana::Draw()
{
	katanaModel_.Draw();

#ifdef _DEBUG
	if (!colliders_.empty())
	{
		CapsuleCollider* pCap = static_cast<CapsuleCollider*>(colliders_[0].get());

		if (pCap)
		{
			VECTOR top = VGet(pCap->GetWorldB().x_, pCap->GetWorldB().y_, pCap->GetWorldB().z_);
			VECTOR bottom = VGet(pCap->GetWorldA().x_, pCap->GetWorldA().y_, pCap->GetWorldA().z_);

			unsigned int lineColor;
			if (isHit_)
			{
				//当たっている場合は赤
				lineColor = GetColor(255, 0, 0);
			}
			else if (IsEnabled())
			{
				//当たり判定が有効な場合は水色
				lineColor = GetColor(255, 0, 0);
			}
			else
			{
				//当たり判定が無効な場合は水色
				lineColor = GetColor(0, 255, 255);
			}

			//描画
			DrawCapsule3D(top, bottom, pCap->GetRadius(), 8, lineColor, GetColor(0, 0, 0), FALSE);
		}
	}
#endif
}

void Katana::SetColliderEnabled(bool isEnabled)
{
	this->SetEnabled(isEnabled);
}
