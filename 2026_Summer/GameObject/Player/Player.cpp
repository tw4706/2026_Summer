#include "Player.h"
#include "Katana.h"
#include "Input.h"
#include "Game.h"
#include "Camera/CameraBase.h"
#include "Math/Matrix4x4.h"
#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateDamage.h"
#include "PlayerStateDeath.h"
#include "Enemy/EnemyBase.h"
#include "Collider/CapsuleCollider.h"
#include "Collider/SphereCollider.h"
#include<Dxlib.h>
#include<memory>
#include<cassert>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f, 0.0f, 0.0f };

	//初期スケール
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//初期回転角度
	const Vector3 kFirstRotate = { 0.0f, DX_PI_F, 0.0f };

	//プレイヤーの最大体力
	constexpr int kMaxHP = 100;

	//敵から食らうダメージ
	constexpr int kEnemyDamage = 10;

	//1フレームの時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//コライダーの半径と高さ
	constexpr float kColliderRadius = 40.0f;
	constexpr float kColliderHeight = 160.0f;
	//コライダーのオフセット
	const Vector3 kColliderOffset = { 0.0f, 120.0f, 0.0f };
	//注視点の高さのオフセット
	constexpr float kCameraTargetHeightOffset = 150.0f;

	//レイ(視線)の高さ
	constexpr float kRayStartHeight = 80.0f;
	//レティクルの倍率
	constexpr float kReticleScale = 0.2f;
	//画面座標のX補正値
	constexpr float kReticleOffsetX = 100.0f;
	//画面座標のY補正値
	constexpr float kReticleOffsetY = 80.0f;           

	//HPバーの描画オフセット
	constexpr int kHPBarOffsetX = 250;
	constexpr int kHPBarOffsetY = 100;
}

Player::Player() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f),
	moveAngle_(0.0f),
	handFrameIndex_(-1),
	hpUIHandle_(-1),
	hpUIFrameHandle_(-1),
	hpUIX_(0),
	hpUIY_(0),
	hpBarUIX_(0),
	hpBarUIY_(0),
	reticleUIHandle_(-1),
	pKatana_(nullptr)
{
}

Player::~Player()
{
	//ハンドルの削除
	DeleteGraph(hpUIHandle_);
	DeleteGraph(hpUIFrameHandle_);
	DeleteGraph(reticleUIHandle_);

	//モデルの削除
	model_.Release();
}

void Player::Init()
{
	//ポジションと速度の初期化
	pos_ = kFirstPos;

	vel_ = { 0.0f,0.0f,0.0f };

	isGround_ = true;

	//HPの初期化
	hp_ = kMaxHP;

	//画像のロード
	//HPUI
	hpUIHandle_ = LoadGraph(L"data/UI/PlayerHP.png");
	assert(hpUIHandle_ >= 0);
	GetGraphSize(hpUIHandle_, &hpUIX_, &hpUIY_);

	//HPUIFrame
	hpUIFrameHandle_ = LoadGraph(L"data/UI/HPBar.png");
	assert(hpUIFrameHandle_ >= 0);
	GetGraphSize(hpUIFrameHandle_, &hpBarUIX_, &hpBarUIY_);

	//レティクルUI
	reticleUIHandle_ = LoadGraph(L"data/UI/reticle.png");
	int reticleX, reticleY;
	GetGraphSize(reticleUIHandle_, &reticleX, &reticleY);

	//モデルのロード
	model_.Load(L"data/MV1/Player.mv1");
	handFrameIndex_ = model_.SearchFrame(L"mixamorig:RightHand");

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());
	animation_.ChangeState(AnimationState::Idle);

	//コライダーの登録
	Vector3 colOffset = kColliderOffset;
	this->CreateCollider<CapsuleCollider>(kColliderRadius, kColliderHeight, colOffset);

	//刀の生成・初期化
	pKatana_ = std::make_unique<Katana>(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f);
	pKatana_->SetOwnerCharacter(this);
	pKatana_->Init();
}

void Player::Update()
{
	isHit_ = false;

	Collidable::Update();

	activeCamera_ = GetActiveCamera();
	if (!activeCamera_)return;

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

	if (!pCurrentState_)
	{
		auto sharedSelf = std::dynamic_pointer_cast<Player>(shared_from_this());
		std::weak_ptr<Player> weakSelf = sharedSelf;

		//ステートパターンの生成
		pCurrentState_ = std::make_shared<PlayerStateIdle>(weakSelf);

		pCurrentState_->Enter();
	}

	if (pCurrentState_)
	{
		pCurrentState_->Update();
	}

	//アニメーションの更新
	animation_.Update(kDeltaTime);

	DrawFormatString(0, 0, GetColor(255, 255, 255), L"Hand Index: %d", handFrameIndex_);
}

void Player::Draw()
{
	//行列で描画位置や回転などの計算
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

	//ロックオンのレティクルUI
	if (IsLockOn())
	{
		//ターゲットのロックができているなら
		if (auto target = pLockOnEnemy_.lock())
		{
			//始点(始点はプレイヤーの位置から高さをプラスしている)
			Vector3 startPos = pos_;
			startPos.y_ += kRayStartHeight;

			//終点(敵の半分くらいの位置)
			Vector3 endPos = target->GetCameraTarget();

			//DxLibのVECTOR型に変換
			VECTOR start = startPos.ToDxlibVector();
			VECTOR end = endPos.ToDxlibVector();

			//ロックオン中は「黄色」の太い線でレイを描画
			unsigned int rayColor = GetColor(255, 255, 0);
			DrawLine3D(start, end, rayColor);

			VECTOR reticleScreenPos = ConvWorldPosToScreenPos(end);

			SetUseZBuffer3D(false);

			//レティクルUIの描画
			DrawRotaGraph3(static_cast<int>(reticleScreenPos.x - kReticleOffsetX), static_cast<int>(reticleScreenPos.y - kReticleOffsetY),
				0, 0,
				kReticleScale, kReticleScale,
				0.0f, reticleUIHandle_, true);

			SetUseZBuffer3D(true);
		}
	}

#ifdef _DEBUG
	if (!colliders_.empty())
	{
		CapsuleCollider* pCap = static_cast<CapsuleCollider*>(colliders_[0].get());

		if (pCap)
		{
			VECTOR top = VGet(pCap->GetWorldB().x_, pCap->GetWorldB().y_, pCap->GetWorldB().z_);
			VECTOR bottom = VGet(pCap->GetWorldA().x_, pCap->GetWorldA().y_, pCap->GetWorldA().z_);

			//当たっていたら赤色通常時は水色
			unsigned int lineColor = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 255);

			//描画
			DrawCapsule3D(top, bottom, pCap->GetRadius(), 8, lineColor, GetColor(0, 0, 0), false);
		}
	}

	//HPのデバッグ表示
	DrawFormatString(100, 150, 0xffffff, L"PlayerHP:%d", hp_);

	//プレイヤーの座標のデバッグ表示
	DrawFormatString(100, 100, 0x00ffff, L"PlayerPosX : %.2f,PlayerPosY : %.2f,PlayerPosZ : %.2f", pos_.x_, pos_.y_, pos_.z_);
#endif

	//HPの割合
	float hpRate = static_cast<float>(hp_) / kMaxHP;
	int drawHPWidth = static_cast<int>(hpUIX_ * hpRate);

	//HPバーフレームの描画
	DrawRectGraph(Game::kScreenWidth / 2 - kHPBarOffsetX, Game::kScreenHeight - kHPBarOffsetY,
		0, 0,
		hpBarUIX_, hpBarUIY_,
		hpUIFrameHandle_, true);

	//HPバーの描画
	DrawRectGraph(Game::kScreenWidth / 2 - kHPBarOffsetX, Game::kScreenHeight - kHPBarOffsetY,
		0, 0,
		drawHPWidth, hpUIY_,
		hpUIHandle_, true);
}

void Player::OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)
{
	//無敵の場合は何もしない
	if (isInvincible_)return;

	//衝突した相手が敵だった場合ダメージ状態に遷移
	if (EnemyBase* pEnemy = dynamic_cast<EnemyBase*>(&coll))
	{
		const SphereCollider* attackCollider = pEnemy->GetAttackCollider();

		//当たったものが攻撃コライダーの場合ならダメージを通す
		if (attackCollider != nullptr &&
			static_cast<const Collider*>(pColliderB) == static_cast<const Collider*>(attackCollider))
		{
			OnDamage(kEnemyDamage);
		}
	}
	isHit_ = true;
}

void Player::OnDamage(const int damage)
{
	//ガード中はダメージを通さない
	if (isGuarding_)
	{
		return;
	}

	//hpを減らす
	hp_ -= damage;

	//無敵状態をtrueに
	isInvincible_ = true;

	auto sharedSelf = std::dynamic_pointer_cast<Player>(shared_from_this());
	std::weak_ptr<Player> weakSelf = sharedSelf;

	//HPが0以上の場合はダメージ状態に遷移
	if (hp_ > 0)
	{
		auto nextState = std::make_shared<PlayerStateDamage>(weakSelf);
		ChangeState(nextState);
	}
	//HPが0の場合は死亡状態に遷移
	else
	{
		hp_ = 0;

		auto nextState = std::make_shared<PlayerStateDeath>(weakSelf);
		ChangeState(nextState);
	}
}

Vector3 Player::GetCameraTarget() const
{
	return Vector3{ pos_.x_,pos_.y_ + kCameraTargetHeightOffset,pos_.z_ };
}

void Player::SetKatanaColliderEnabled(bool isEnabled)
{
	if (pKatana_)
	{
		pKatana_->SetColliderEnabled(isEnabled);
	}
}

void Player::PlayKatanaEffect()
{
	//刀のエフェクト再生
	pKatana_->PlayEffect();
}

void Player::StopKatanaEffect()
{
	if (pKatana_) pKatana_->StopEffect();
}
