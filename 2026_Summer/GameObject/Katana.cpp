#include "Katana.h"
#include "Animation.h"
#include "Enemy/EnemyBase.h"
#include "Collider/CapsuleCollider.h"
#include "EffectManager.h"
#include<EffekseerForDXLib.h>
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
	constexpr int kAttackDamage = 10;

	//コライダーの半径
	constexpr float kColliderRadius = 10.0f;

	//コライダーの高さ
	constexpr float kColliderHeight = 60.0f;
}

Katana::Katana(Vector3 pos, Vector3 vel, float dir) :
	Collidable(pos, vel, dir),
	worldMat_(0)
{
}

Katana::~Katana()
{
	katanaModel_.Release();
}

void Katana::Init()
{
	//刀モデルのロード
	katanaModel_.Load(L"data/MV1/Tachi.mv1");

	//エフェクトのロード
	EffectManager::GetInstance().Load(L"Slash", L"data/Effect/KatanaFrame.efk");
	
	auto pCapsule = std::make_unique<CapsuleCollider>(kColliderRadius, kColliderHeight, Vector3{ 0.0f, 0.0f, 0.0f });
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

	//worldMat_を使ってプレイヤーのpos_を刀の位置に合成させる
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
			pCapsule->SetWorldPos(worldStart,worldEnd);
		}
	}

	Vector3 katanaWorldPos = 
	{
		worldMat_.m[3][0],
		worldMat_.m[3][1],
		worldMat_.m[3][2]
	};

	if (currentEffectHandle_ != -1)
	{
		SetPosPlayingEffekseer3DEffect(currentEffectHandle_, worldEnd.x, worldEnd.y, worldEnd.z);
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

void Katana::OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)
{
	//当たり判定が無効な時は何もしない
	if (!IsEnabled())return;

	//攻撃がヒットしてるなら何もしない
	if (isAttacked_) return;

	//フラグをtrueにする
	isHit_ = true;

	//敵かどうかを判定してダメージを与える
	EnemyBase* pEnemy = dynamic_cast<EnemyBase*>(&coll);
	if (pEnemy)
	{
		pEnemy->OnDamage(kAttackDamage);
		pEnemy->OnCollision(*this,pColliderA,pColliderB);
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

void Katana::PlayEffect()
{
	//刀の最新のワールド座標
	Vector3 katanaWorldPos =
	{
		worldMat_.m[3][0],
		worldMat_.m[3][1],
		worldMat_.m[3][2]
	};

	currentEffectHandle_ = EffectManager::GetInstance().Play(L"Slash", katanaWorldPos);
}

void Katana::StopEffect()
{
	if (currentEffectHandle_ != -1)
	{
		EffectManager::GetInstance().Stop(currentEffectHandle_);
		currentEffectHandle_ = -1;
	}
}
