#include "EnemyBase.h"
#include "Player/Player.h"
#include "Katana.h"
#include "Game.h"
#include "EnemyManager.h"
#include "EnemyStateIdle.h"
#include "EnemyStateDamage.h"
#include "EnemyStateDeath.h"
#include "CollisionManager.h"
#include"Collider/CapsuleCollider.h"
#include"Collider/SphereCollider.h"
#include<cassert>


EnemyBase::EnemyBase() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f),
	moveAngle_(0.0f),
	scale_({ 1.0f,1.0f,1.0f }),
	searchRadius_(500.0f),
	colliderRadius_(70.0f),
	colliderHeight_(120.0f),
	pathFinder_(),
	pAttackCollider_(nullptr),
	attackColliderDistance_(0.0f)

{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Init()
{
	vel_ = { 0.0f, 0.0f, 0.0f };
	isHit_ = false;

	//画像のロード
	hpHandle_ = LoadGraph(L"data/UI/EnemyHP.png");
	assert(pHandle_ >= 0);
	GetGraphSize(hpHandle_, &hpUIX_, &hpUIY_);

	hpFrameHandle_ = LoadGraph(L"data/UI/HPBar.png");
	assert(hpFrameHandle_ >= 0);
	GetGraphSize(hpFrameHandle_, &hpBarUIX_, &hpBarUIY_);

	//アニメーションの初期化
	animation_.Init(model_.GetHandle());

	//初期アニメーション
	animation_.ChangeState(AnimationState::Idle);

	//コライダー生成
	Vector3 colOffset = Vector3{ 0.0f,colliderHeight_,0.0f };
	this->CreateCollider<CapsuleCollider>(colliderRadius_, colliderHeight_, colOffset);
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
		ChangeState(std::make_shared<EnemyStateIdle>(enemy, searchRadius_));
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
			//高さの半分
			float halfH = colliderHeight_ * 0.5f;

			Vector3 centerPos = pos_ + Vector3{ 0.0f, colliderHeight_, 0.0f };
			Vector3 top = centerPos + Vector3{ 0.0f, halfH, 0.0f };
			Vector3 bottom = centerPos + Vector3{ 0.0f, -halfH, 0.0f };
			pCap->SetWorldPos(bottom, top);
		}
	}

	//攻撃コライダーの更新
	if (pAttackCollider_)
	{
		Vector3 forward = { sinf(moveAngle_), 0.0f, -cosf(moveAngle_) };
		Vector3 offset = pos_ + forward * attackColliderDistance_ + Vector3{ 0.0f, colliderHeight_ * 0.5f, 0.0f };
		pAttackCollider_->SetPos(offset);
	}
}

void EnemyBase::Draw()
{
	//モデルの描画
	model_.Draw();
#ifdef _DEBUG
	//索敵範囲のデバッグ表示
	Vector3 center = pos_;
	Vector3 playerPos = GetPlayerPos();
	Vector3 toPlayer = playerPos - pos_;
	float distSq = toPlayer.LengthSq();

	unsigned int searchColor = GetColor(0, 255, 0);
	if (distSq <= searchRadius_ * searchRadius_)
	{
		searchColor = GetColor(255, 0, 0);
	}
	DrawSphere3D(center.ToDxlibVector(), searchRadius_, 8, searchColor, GetColor(0, 0, 0), FALSE);

	//当たり判定のデバッグ表示
	for (const auto& pCol : colliders_)
	{
		if (CapsuleCollider* pCap = dynamic_cast<CapsuleCollider*>(pCol.get()))
		{
			Vector3 top = pCap->GetWorldB();
			Vector3 bottom = pCap->GetWorldA();
			unsigned int lineColor = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 255);
			DrawCapsule3D(top.ToDxlibVector(), bottom.ToDxlibVector(), pCap->GetRadius(), 8, lineColor, GetColor(0, 0, 0), FALSE);
		}
		else if (SphereCollider* pSphere = dynamic_cast<SphereCollider*>(pCol.get()))
		{
			//攻撃コライダーはオレンジ色で表示
			DrawSphere3D(pSphere->GetPos().ToDxlibVector(), pSphere->GetRadius(), 8, GetColor(255, 128, 0), GetColor(0, 0, 0), FALSE);
		}
	}

	//経路探索のデバッグ表示
	if (hasDebugTarget_)
	{
		Vector3 enemyPos = GetPos();
		Vector3 startPos = { enemyPos.x_, enemyPos.y_ + 10.0f, enemyPos.z_ };
		Vector3 endPos = { debugNextPos_.x_, debugNextPos_.y_ + 10.0f, debugNextPos_.z_ };

		unsigned int colorLine = GetColor(255, 0, 0);
		DrawLine3D(startPos.ToDxlibVector(), endPos.ToDxlibVector(), colorLine);
	}
	if (pWayPointLoader_)
	{
		//敵と同じエリアIDのWayPointを取得
		const auto& wayPoints = pWayPointLoader_->GetWayPoints(areaId_);

		for (const auto& wp : wayPoints)
		{
			Vector3 wayPointPos = { wp.pos.x_, wp.pos.y_ + 10.0f, wp.pos.z_ };

			//通常のWayPointは青色にする
			unsigned int wayPointColor = GetColor(0, 0, 255);

			//もしこのWayPointが、現在敵が目指しているターゲットIDと同じなら黄色にする
			if (hasDebugTarget_ && wp.id == nextWayPointId_)
			{
				wayPointColor = GetColor(255, 255, 0);
			}

			//WayPointの位置に球を描画
			DrawSphere3D(wayPointPos.ToDxlibVector(), 15.0f, 8, wayPointColor, wayPointColor, TRUE);
		}
	}
#endif

	//敵の頭上の3D座標を計算
	Vector3 headWorldPos = GetCameraTarget() + Vector3{ 0.0f, 70.0f, 0.0f };

	//D座標を画面の2D座標に変換
	VECTOR screenPos = ConvWorldPosToScreenPos(headWorldPos.ToDxlibVector());

	//画面外にいる場合は描画しない判定
	//変換結果の Z 値が 0.0f ～ 1.0f の間にあれば画面内に映っています
	if (screenPos.z >= 0.0f && screenPos.z <= 1.0f)
	{
		//拡大率
		float scale = 0.2f;

		//HPの割合
		float hpRate = static_cast<float>(hp_) / maxHP_;
		int drawHPWidth = static_cast<int>(hpUIX_ * hpRate);

		int scaledBarW = static_cast<int>(hpBarUIX_ * scale); //フレームの幅
		int scaledBarH = static_cast<int>(hpBarUIY_ * scale); //フレームの高さ
		int scaledHPW = static_cast<int>(drawHPWidth * scale); //バーの幅

		//HPバーの基準点を計算
		int drawX = static_cast<int>(screenPos.x) - (scaledBarW / 2);
		int drawY = static_cast<int>(screenPos.y) - (scaledBarH / 2);

		//HPバーフレームの描画
		DrawRectExtendGraph(drawX, drawY,
			drawX+ scaledBarW, drawY+ scaledBarH, 0, 0,
			hpBarUIX_, hpBarUIY_,
			hpFrameHandle_, true);

		//HPバーの描画
		DrawRectExtendGraph(drawX, drawY,
			drawX + scaledHPW, drawY + scaledBarH, 0, 0,
			drawHPWidth, hpUIY_,
			hpHandle_, true);
	}
}

void EnemyBase::OnCollision(Collidable& coll)
{
	//衝突相手の型が刀かどうかをチェック
	if (Katana* pKatana = dynamic_cast<Katana*>(&coll))
	{
		//刀の当たり判定が無効な場合は何もしない
		if (!pKatana->IsEnabled())
		{
			return;
		}

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
		auto nextState = std::make_shared<EnemyStateDamage>(enemy, searchRadius_);
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
		auto nextState = std::make_shared<EnemyStateDeath>(enemy, searchRadius_);
		ChangeState(nextState);
	}
}

void EnemyBase::CreateAttackCollider(float radius, float distance)
{
	//攻撃コライダーが既に存在する場合は何もしない
	if (pAttackCollider_)return;

	attackColliderDistance_ = distance;

	pAttackCollider_ = this->CreateCollider<SphereCollider>(radius);

	Vector3 forward = { sinf(moveAngle_),0.0f,cosf(moveAngle_) };
	Vector3 offset = pos_ + forward * attackColliderDistance_ + Vector3{ 0.0f, colliderHeight_ * 0.5f, 0.0f };

	pAttackCollider_->SetPos(offset);
}

void EnemyBase::RemoveAttackCollider()
{
	if (!pAttackCollider_) return;

	CollisionManager::GetInstance().UnRegisterCollider(pAttackCollider_);

	auto it = std::find_if(colliders_.begin(), colliders_.end(),
		[this](const std::unique_ptr<Collider>& pCol)
		{
			return pCol.get() == pAttackCollider_;
		});

	if (it != colliders_.end())
	{
		colliders_.erase(it);
	}

	pAttackCollider_ = nullptr;
}

void EnemyBase::ApplyData(const EnemyData& data, const EnemySpawnData& spawnData, const WayPointLoader* pWayPointLoader)
{
	//WayPointのローダー
	pWayPointLoader_ = pWayPointLoader;

	//エリアID
	areaId_ = spawnData.areaId_;

	//ステータス
	hp_ = spawnData.hp_;

	//最大体力
	maxHP_= spawnData.hp_;

	//トランスフォーム
	pos_ = spawnData.spawnPos_;
	moveAngle_ = spawnData.rotateY;
	scale_ = spawnData.scale_;

	//索敵範囲
	searchRadius_ = data.searchRadius_;

	//コライダーの半径と高さ
	colliderRadius_ = data.colliderRadius_;
	colliderHeight_ = data.colliderHeight_;

	//モデルの読み込み
	if (!data.modelPath_.empty())
	{
		model_.Load(data.modelPath_.c_str());
		assert(model_.GetHandle() >= 0);
	}

	//アニメーション名をアニメーションクラスに登録
	animation_.RegisterAnimName(AnimationState::Idle, data.chanceAnim_);
	animation_.RegisterAnimName(AnimationState::Walk, data.walkAnim_);
	animation_.RegisterAnimName(AnimationState::Run, data.runAnim_);
	animation_.RegisterAnimName(AnimationState::EnemyAttack, data.attackAnim_);
	animation_.RegisterAnimName(AnimationState::EnemyJumpAttack, data.jumpAttackAnim_);
	animation_.RegisterAnimName(AnimationState::Damage, data.damageAnim_);
	animation_.RegisterAnimName(AnimationState::Death, data.deathAnim_);
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

Vector3 EnemyBase::GetCameraTarget() const
{
	return pos_ + Vector3{ 0.0f,colliderHeight_,0.0f };
}

void EnemyBase::SetPlayer(std::weak_ptr<Player> pPlayer)
{
	pPlayer_ = pPlayer;
}

const PlayerActionCounter* EnemyBase::GetPlayerActionCounter() const
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer)
	{
		return nullptr;
	}
	//プレイヤーのカウンタを返す
	return &pPlayer->GetActionCounter();
}

void EnemyBase::SetNavigationGrid(const NavigationGrid* pNavGrid)
{
	pNaviGrid_ = pNavGrid;
	pathFinder_.SetNavigationGrid(pNavGrid);
}

void EnemyBase::SetSlowAnimationSpeed()
{
	animation_.SetSlowAnimationSpeed();
}

void EnemyBase::SetAttackAnimationSpeed()
{
	animation_.SetEnemyAttackAnimationSpeed();
}
