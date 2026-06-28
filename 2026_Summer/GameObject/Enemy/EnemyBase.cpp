#include "EnemyBase.h"
#include "Player/Player.h"
#include "Katana.h"
#include "EnemyStateDamage.h"
#include "EnemyStateDeath.h"
#include "EnemyStateIdle.h"
#include"Collider/CapsuleCollider.h"
#include<cassert>


EnemyBase::EnemyBase() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f),
	moveAngle_(0.0f),
	scale_({1.0f,1.0f,1.0f})
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Init()
{
	vel_ = { 0.0f, 0.0f, 0.0f };
	isHit_ = false;

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());
}

void EnemyBase::Update()
{
	// Collidableクラスの更新
	Collidable::Update();

	//ステートが入ってない場合
	if (!pCurrentState_)
	{
		//EnemyBaseとして弱参照を作る
		auto enemy = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());
		ChangeState(std::make_shared<EnemyStateIdle>(enemy));
	}

	//現在のステートの更新
	if (pCurrentState_)
	{
		pCurrentState_->Update();
	}

	//アニメーションの更新
	animation_.Update(1.0f / 60.0f);

	//行列の計算
	Matrix4x4 scaleMat = Matrix4x4::Scale(scale_.x_, scale_.y_, scale_.z_);
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_);
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);
	Matrix4x4 worldMat = scaleMat * rotMat * transMat;

	//モデルに行列をセット
	model_.SetMatrix(worldMat);

	if (!colliders_.empty())
	{
		CapsuleCollider* pCap = static_cast<CapsuleCollider*>(colliders_[0].get());
		if (pCap)
		{
			Vector3 centerPos = pos_ + Vector3{ 0.0f, 120.0f, 0.0f };
			Vector3 top = centerPos + Vector3{ 0.0f, 60.0f, 0.0f };
			Vector3 bottom = centerPos + Vector3{ 0.0f, -60.0f, 0.0f };
			pCap->SetWorldPos(bottom, top);
		}
	}
}

void EnemyBase::Draw()
{
	//モデルの描画
	model_.Draw();

#ifdef _DEBUG
	//敵の頭上にHPをデバッグ表示
	DrawFormatString(static_cast<int>(pos_.x_), static_cast<int>(pos_.y_ + 150.0f), GetColor(255, 255, 255), L"HP: %d", hp_);
#endif
}

void EnemyBase::OnCollision(Collidable& coll)
{
	//衝突相手の型が刀かどうかをチェック
	if (Katana* pKatana = dynamic_cast<Katana*>(&coll))
	{
		//すでにダメージ状態なら何もしない
		if (std::dynamic_pointer_cast<EnemyStateDamage>(pCurrentState_))
		{
			return;
		}

		//すでに死亡状態なら何もしない
		if (std::dynamic_pointer_cast<EnemyStateDeath>(pCurrentState_))
		{
			return;
		}

		auto enemy = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());

		//ダメージ状態に遷移する
		auto nextState = std::make_shared<EnemyStateDamage>(enemy);
		ChangeState(nextState);
	}
	else
	{
		isHit_ = true;
	}
}

void EnemyBase::OnDamage(int damage)
{
	hp_ -= damage;

	if (hp_ <= 0)
	{
		hp_ = 0;

		auto enemy = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());

		//ダメージ状態に遷移する
		auto nextState = std::make_shared<EnemyStateDeath>(enemy);
		ChangeState(nextState);
	}
}

Vector3 EnemyBase::GetPlayerPos() const
{
	auto pPlayer = pPlayer_.lock();

	//もしプレイヤーが存在しない場合は0を返す
	if (!pPlayer)
	{
		return Vector3{ 0.0f,0.0f,0.0f };
	}

	//そうでない場合はプレイヤーの位置を返す
	return pPlayer->GetPos();
}

void EnemyBase::SetPlayer(std::weak_ptr<Player> pPlayer)
{
	pPlayer_ = pPlayer;
}
