#include "Player.h"
#include "Katana.h"
#include"Input.h"
#include"Camera/PlayerCamera.h"
#include"Matrix4x4.h"
#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateDamage.h"
#include "PlayerStateDeath.h"
#include "Enemy/EnemyBase.h"
#include "Collider/CapsuleCollider.h"
#include<Dxlib.h>
#include<memory>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f, 0.0f, 0.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//初期回転角度
	const Vector3 kFirstRotate = { 0.0f, DX_PI_F, 0.0f };

	//プレイヤーの最大体力
	constexpr int kMaxHP = 50;

	//敵から食らうダメージ
	constexpr int kEnemyDamage = 10;
}

Player::Player() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f),
	moveAngle_(0.0f),
	handFrameIndex_(-1),
	pKatana_(nullptr)
{
}

Player::~Player()
{
}

void Player::Init()
{
	//ポジションと速度の初期化
	pos_ = kFirstPos;

	vel_ = { 0.0f,0.0f,0.0f };

	isGround_ = true;

	//HPの初期化
	hp_ = kMaxHP;

	//モデルのロード
	model_.Load(L"data/MV1/Player.mv1");
	handFrameIndex_ = model_.SearchFrame(L"mixamorig:RightHand");

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());
	animation_.ChangeState(AnimationState::Idle);

	//コライダーの登録
	Vector3 colOffset = { 0.0f, 120.0f, 0.0f };
	this->CreateCollider<CapsuleCollider>(40.0f, 160.0f, colOffset);

	//刀の生成・初期化
	pKatana_ = std::make_unique<Katana>(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f);
	pKatana_->SetOwnerCharacter(this);
	pKatana_->Init();
}

void Player::Update()
{
	isHit_ = false;

	Collidable::Update();

	if (!pCurrentState_ && pCamera_)
	{
		auto sharedSelf = std::dynamic_pointer_cast<Player>(shared_from_this());
		std::weak_ptr<Player> weakSelf = sharedSelf;

		//今持っているポインタを参照する
		PlayerCamera& pCamera = *pCamera_;

		//ステートパターンの生成
		pCurrentState_ = std::make_shared<PlayerStateIdle>(weakSelf, *pCamera_);

		pCurrentState_->Enter();
	}

	if (pCurrentState_)
	{
		pCurrentState_->Update();
	}

	animation_.Update(1.0f / 60.0f);

	//行列を作成
	//拡縮
	Matrix4x4 scaleMat = Matrix4x4::Scale(kFirstScale.x_, kFirstScale.y_, kFirstScale.z_);
	//Y軸回転
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_);
	//移動
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	//行列の合成
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;

	//モデルに行列をセット
	model_.SetMatrix(worldMat);

	//刀の更新（手のフレーム行列を渡す）
	if (handFrameIndex_ != -1 && pKatana_)
	{
		MATRIX handMat = MV1GetFrameLocalWorldMatrix(model_.GetHandle(), handFrameIndex_);
		pKatana_->Update(handMat, animation_.GetState());
	}

	DrawFormatString(0, 0, GetColor(255, 255, 255), L"Hand Index: %d", handFrameIndex_);
}

void Player::Draw()
{
	Matrix4x4 scaleMat = Matrix4x4::Scale(kFirstScale.x_, kFirstScale.y_, kFirstScale.z_);
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_);
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;
	model_.SetMatrix(worldMat);

	model_.Draw();

	//刀の描画
	if (pKatana_)
	{
		pKatana_->Draw();
	}

#ifdef _DEBUG
	if (!colliders_.empty())
	{
		CapsuleCollider* pCap = static_cast<CapsuleCollider*>(colliders_[0].get());

		if (pCap)
		{
			VECTOR top = VGet(pCap->GetWorldB().x_, pCap->GetWorldB().y_, pCap->GetWorldB().z_);
			VECTOR bottom = VGet(pCap->GetWorldA().x_, pCap->GetWorldA().y_, pCap->GetWorldA().z_);

			//当たっていたら赤（255,0,0）、通常時は水色
			unsigned int lineColor = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 255);

			//描画
			DrawCapsule3D(top, bottom, pCap->GetRadius(), 8, lineColor, GetColor(0, 0, 0), FALSE);
		}
	}

	//HPのデバッグ表示
	DrawFormatString(100, 100, 0xffffff, L"PlayerHP:%d", hp_);
#endif
}

void Player::OnCollision(Collidable& coll)
{
	//無敵の場合は何もしない
	if (isInvincible_)return;

	//衝突した相手が敵だった場合ダメージ状態に遷移
	if (EnemyBase* pEnemy = dynamic_cast<EnemyBase*>(&coll))
	{
		if (pEnemy->GetAttackCollider())
		{
			OnDamage(kEnemyDamage);
		}
	}
	isHit_ = true;


}

void Player::OnDamage(const int damage)
{
	//hpを減らす
	hp_ -= damage;

	//無敵状態をtrueに
	isInvincible_ = true;

	auto sharedSelf = std::dynamic_pointer_cast<Player>(shared_from_this());
	std::weak_ptr<Player> weakSelf = sharedSelf;

	PlayerCamera& pCamera = *pCamera_;

	//HPが0以上の場合はダメージ状態に遷移
	if (hp_ > 0)
	{
		auto nextState = std::make_shared<PlayerStateDamage>(weakSelf, pCamera);
		ChangeState(nextState);
	}
	//HPが0の場合は死亡状態に遷移
	else
	{
		hp_ = 0;

		auto nextState = std::make_shared<PlayerStateDeath>(weakSelf, pCamera);
		ChangeState(nextState);
	}
}

Vector3 Player::GetCameraTarget() const
{
	return Vector3{ pos_.x_,pos_.y_ + 150.0f,pos_.z_ };
}

void Player::SetKatanaColliderEnabled(bool isEnabled)
{
	if (pKatana_)
	{
		pKatana_->SetColliderEnabled(isEnabled);
	}
}
