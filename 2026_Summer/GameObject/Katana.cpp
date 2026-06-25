#include "Katana.h"
#include "Animation.h"
#include "Enemy/EnemyBase.h"
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

	//刀のダメージ
	constexpr int kAttackDamage = 10.0f;
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
	pCapsule->SetUseWorldPos(true);
	this->AddCollider(std::move(pCapsule));
}

void Katana::Update(const MATRIX& handMat, AnimationState ownerState)
{
	Collidable::Update();

	isHit_ = false;

	Vector3 katanaScale = kKatanaScale;
	Vector3 katanaRotate = kKatanaRotate;
	Vector3 katanaTransform = kKatanaTransform;

	if (ownerState == AnimationState::Run)
	{
		katanaRotate.x_ += DX_PI_F / 4.0f;
		katanaRotate.z_ += DX_PI_F / 3.0f;
	}

	// 行列を用いた計算
	Matrix4x4 scaleMat = Matrix4x4::Scale(katanaScale.x_, katanaScale.y_, katanaScale.z_);
	Matrix4x4 rotMat = Matrix4x4::RotateX(katanaRotate.x_) * Matrix4x4::RotateY(katanaRotate.y_) * Matrix4x4::RotateZ(katanaRotate.z_);
	Matrix4x4 transMat = Matrix4x4::Translate(katanaTransform.x_, katanaTransform.y_, katanaTransform.z_);
	Matrix4x4 mat = scaleMat * rotMat * transMat;

	worldMat_ = MMult(mat.ToDxLibMatrix(), handMat);
	katanaModel_.SetMatrix(worldMat_);

	//当たり判定の更新：worldMat_を使って自分のpos_を刀の位置に合成させる
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
	//モデルの描画
	katanaModel_.Draw();
#ifdef _DEBUG
	if (!colliders_.empty())
	{
		CapsuleCollider* pDebugCapsule = static_cast<CapsuleCollider*>(colliders_[0].get());

		if (pDebugCapsule)
		{
			VECTOR top = VGet(pDebugCapsule->GetWorldB().x_, pDebugCapsule->GetWorldB().y_, pDebugCapsule->GetWorldB().z_);
			VECTOR bottom = VGet(pDebugCapsule->GetWorldA().x_, pDebugCapsule->GetWorldA().y_, pDebugCapsule->GetWorldA().z_);

			unsigned int lineColor;
			if (isHit_)
			{
				//当たっている場合は赤色
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
			DrawCapsule3D(top, bottom, pDebugCapsule->GetRadius(), 8, lineColor, GetColor(0, 0, 0), FALSE);
		}
	}
#endif
}

void Katana::OnCollision(Collidable* coll)
{
	//当たり判定が無効な時は何もしない
	if (!IsEnabled())return;

	//攻撃がヒットしてるなら何もしない
	if (isAttacked_) return;

	//フラグをtrueにする
	isHit_ = true;

	//敵かどうかを判定してダメージを与える
	EnemyBase* pEnemy = dynamic_cast<EnemyBase*>(coll);
	if (pEnemy)
	{
		pEnemy->OnDamage(kAttackDamage);
		isAttacked_ = true;
	}
}

void Katana::SetColliderEnabled(bool isEnabled)
{
	this->SetEnabled(isEnabled);

	if (isEnabled)
	{
		isAttacked_ = false;
	}
}
