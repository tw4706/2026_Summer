#include "EnemyBase.h"
#include "Player/Player.h"
#include "Katana.h"
#include "Game.h"
#include "EnemyManager.h"
#include "EnemyStateIdle.h"
#include "EnemyStateDamage.h"
#include "EnemyStateAttack.h"
#include "EnemyStateDeath.h"
#include "EnemyStateReact.h"
#include "CollisionManager.h"
#include "EffectManager.h"
#include"Collider/CapsuleCollider.h"
#include"Collider/SphereCollider.h"
#include<cassert>

namespace
{
	//ダメージに行く確率
	constexpr int kRandomToDamage = 20;

	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//描画時のlerpのrate
	constexpr float kDrawLerpRate = 0.1f;

	//反応行動を開始する範囲
	constexpr float kSearchReactRange = 1000.0f;

	//敵の見ている視野角
	constexpr float kVisionAngle = 90.0f;

	//攻撃コライダーのデバッグDivNum
	constexpr int kDebugAttackColliderDivNum = 8;

	//HPUIの見えている時間
	constexpr float kHPUIVisibleTime = 3.0f;

	//HPUIが消える時の条件の一つであるHPの基準
	constexpr float kDrawVisibleMinHP = 0.1f;

	//ヒットエフェクトを表示する座標のオフセット
	const Vector3 kHitEffectOffset = { 0.0f,70.0f,0.0f };

	//デバッグ描画用のY座標オフセット
	constexpr float kDrawDebugEnemyOffsetY = 10.0f;

	//wayPointの描画用の半径
	constexpr float kDrawWayPointRadius = 15.0f;

	//WayPointPosのステージ上に見えるように少し上にあげるためのオフセット
	constexpr float kWayPointPosY = 10.0f;

	//ラジアンに変換する定数
	constexpr float kRadian = (DX_PI_F / 180.0f);

	//パーセンテージのマックス
	constexpr int kRandMax = 100;

	//分割数
	constexpr int kDivNum = 10;

	//当たり判定の描画用Div
	constexpr int kDrawColliderDiv = 8;

	//wayPointの描画用のDiv
	constexpr int kDrawWayPointDiv = 8;

	//索敵範囲のデバッグ描画用のDiv
	constexpr int kDrawDebugRangeDiv = 16;
}

EnemyBase::EnemyBase() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f),
	moveAngle_(0.0f),
	scale_({ 1.0f,1.0f,1.0f }),
	searchRadius_(0.0f),
	colliderRadius_(0.0f),
	colliderHeight_(0.0f),
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

	//エフェクトのロード
	EffectManager::GetInstance().Load(L"Hit", L"data/Effect/HitEffect.efk");
	EffectManager::GetInstance().Load(L"EnemyAttack", L"data/Effect/EnemyAttack.efk");

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
	//Collidableクラスの更新
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
	animation_.Update(kDeltaTime);

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
			float halfH = colliderHeight_ * Game::kHalf;

			Vector3 centerPos = pos_ + Vector3{ 0.0f, colliderHeight_, 0.0f };
			Vector3 top = centerPos + Vector3{ 0.0f, halfH, 0.0f };
			Vector3 bottom = centerPos + Vector3{ 0.0f, -halfH, 0.0f };
			pCap->SetWorldPos(bottom, top);
		}
	}

	//攻撃コライダーの更新
	if (pAttackCollider_)
	{
		if (isRangedAttack_)
		{
			//保存しておいた方向へ直進させる
			Vector3 moveVec = rangedAttackDir_ * rangedAttackSpeed_ * kDeltaTime;
			Vector3 nextPos = pAttackCollider_->GetPos() + moveVec;
			pAttackCollider_->SetPos(nextPos);
		}
		else
		{
			Vector3 forward = { sinf(moveAngle_), 0.0f, -cosf(moveAngle_) };
			Vector3 offset = pos_ + forward * attackColliderDistance_ + Vector3{ 0.0f, colliderHeight_ * Game::kHalf, 0.0f };
			pAttackCollider_->SetPos(offset);
		}
	}
}

void EnemyBase::Draw()
{
	//モデルの描画
	model_.Draw();
#ifdef _DEBUG

	//索敵範囲のデバッグ表示
	DrawDebugSearchRange(pos_, kSearchReactRange, 0xffff00);

	//視野角のデバッグ描画をすべき場合は
	if (ShouldDrawVisionDebug())
	{	
		//敵の視野角範囲のデバッグ描画
		float visionDist = searchRadius_;		//半径
		float visionAngle = kVisionAngle;       //視野角の設定

		Vector3 playerPos = GetPlayerPos();

		//プレイヤーが扇状の視界に入っているかで色を変える
		//最初は緑
		unsigned int searchColor = Game::kGreenColor;
		if (IsPlayerInVision(visionDist, visionAngle))
		{
			searchColor = Game::kRedColor; //見つけたら赤色
		}

		//左右の線の描画
		float halfAngleRad = (visionAngle * Game::kHalf) * kRadian;

		//正面から左右に視野角の半分回転させた方向の単位ベクトル
		Vector3 leftDir = { sinf(moveAngle_ - halfAngleRad), 0.0f, -cosf(moveAngle_ - halfAngleRad) };
		Vector3 rightDir = { sinf(moveAngle_ + halfAngleRad), 0.0f, -cosf(moveAngle_ + halfAngleRad) };

		//敵の位置からその方向に視界距離だけ伸びた点を計算
		Vector3 leftLineEnd = pos_ + leftDir * visionDist;
		Vector3 rightLineEnd = pos_ + rightDir * visionDist;

		//3D空間に線を描画
		Vector3 drawOffset = { 0.0f, 0.0f, 0.0f };
		DrawLine3D((pos_ + drawOffset).ToDxlibVector(), (leftLineEnd + drawOffset).ToDxlibVector(), searchColor);
		DrawLine3D((pos_ + drawOffset).ToDxlibVector(), (rightLineEnd + drawOffset).ToDxlibVector(), searchColor);

		//先端の円弧の描画
		//左右の線の先端の間をさらにいくつかの線で繋いで円を作る
		for (int i = 0; i < kDivNum; ++i)
		{
			//左右の角度の間を線形補間する
			float angleA = (moveAngle_ - halfAngleRad) + (visionAngle * kRadian / kDivNum) * i;
			float angleB = (moveAngle_ - halfAngleRad) + (visionAngle * kRadian / kDivNum) * (i + 1);

			Vector3 dirA = { sinf(angleA), 0.0f, -cosf(angleA) };
			Vector3 dirB = { sinf(angleB), 0.0f, -cosf(angleB) };

			Vector3 posA = pos_ + dirA * visionDist + drawOffset;
			Vector3 posB = pos_ + dirB * visionDist + drawOffset;

			DrawLine3D(posA.ToDxlibVector(), posB.ToDxlibVector(), searchColor);
		}
	}

	//当たり判定のデバッグ表示
	for (const auto& pCol : colliders_)
	{
		if (CapsuleCollider* pCap = dynamic_cast<CapsuleCollider*>(pCol.get()))
		{
			Vector3 top = pCap->GetWorldB();
			Vector3 bottom = pCap->GetWorldA();
			unsigned int lineColor = isHit_ ? Game::kRedColor : Game::kLightBlueColor;
			DrawCapsule3D(top.ToDxlibVector(), bottom.ToDxlibVector(), pCap->GetRadius(), kDrawColliderDiv, lineColor, GetColor(0, 0, 0), false);
		}
		else if (SphereCollider* pSphere = dynamic_cast<SphereCollider*>(pCol.get()))
		{
			//攻撃コライダーはオレンジ色で表示
			DrawSphere3D(pSphere->GetPos().ToDxlibVector(), pSphere->GetRadius(), kDebugAttackColliderDivNum, Game::kOrangeColor, GetColor(0, 0, 0), false);
		}
	}

	//経路探索のデバッグ表示
	//ナビゲーショングリッドが存在するなら
	if (pNavigation_)
	{
		if (pNavigation_->HasDebugTarget())
		{
			Vector3 enemyPos = GetPos();
			Vector3 startPos = { enemyPos.x_, enemyPos.y_ + kDrawDebugEnemyOffsetY, enemyPos.z_ };
			const Vector3& debugNextPos = pNavigation_->GetDebugNextPos();
			Vector3 endPos = { debugNextPos.x_, debugNextPos.y_ + kDrawDebugEnemyOffsetY, debugNextPos.z_ };

			unsigned int colorLine = Game::kRedColor;
			DrawLine3D(startPos.ToDxlibVector(), endPos.ToDxlibVector(), colorLine);
		}

		if (const WayPointLoader* pWayPointLoader = pNavigation_->GetWayPointLoader())
		{
			//敵と同じエリアIDのWayPointを取得
			const auto& wayPoints = pWayPointLoader->GetWayPoints(pNavigation_->GetAreaId());

			for (const auto& wp : wayPoints)
			{
				Vector3 wayPointPos = { wp.pos.x_, wp.pos.y_ + kWayPointPosY, wp.pos.z_ };

				//デフォルトは青色
				unsigned int wayPointColor = Game::kBlueColor;

				//WayPointが敵が目指しているターゲットIDと同じ場合黄色
				if (pNavigation_->HasDebugTarget() && wp.id == pNavigation_->GetNextWayPointId())
				{
					wayPointColor = Game::kYellowColor;
				}

				//WayPointを球で描画
				DrawSphere3D(wayPointPos.ToDxlibVector(), kDrawWayPointRadius, kDrawWayPointDiv, wayPointColor, wayPointColor, true);
			}
		}
	}
#endif
}

void EnemyBase::OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)
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

		//攻撃状態なら入らない
		if (std::dynamic_pointer_cast<EnemyStateAttack>(pCurrentState_))
		{
			return;
		}

		auto enemy = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());

		int rand = std::rand() % kRandMax;

		//20パーセントの確率でダメージアニメーションに遷移する
		if (rand < kRandomToDamage)
		{
			//ダメージ状態に遷移する
			auto nextState = std::make_shared<EnemyStateDamage>(enemy, searchRadius_);
			ChangeState(nextState);
		}
	}
	else
	{
		isHit_ = true;
	}
}

void EnemyBase::OnDamage(int damage)
{
	hp_ -= damage;

	//被弾したらHPUIを出現させる
	if (auto pGauge = pHPGaugeUI_.lock())
	{
		pGauge->VisbleDamage();
	}

	Vector3 effectPos = pos_ + kHitEffectOffset;

	//エフェクトの再生
	EffectManager::GetInstance().Play(L"Hit", effectPos);

	//HPが0の場合
	if (hp_ <= 0)
	{
		hp_ = 0;

		auto enemy = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());

		//死亡状態に遷移する
		auto nextState = std::make_shared<EnemyStateDeath>(enemy, searchRadius_);
		ChangeState(nextState);
	}
}

void EnemyBase::CreateAttackCollider(float radius, float distance,int attackDamage)
{
	//攻撃コライダーが既に存在する場合は何もしない
	if (pAttackCollider_)return;

	attackColliderDistance_ = distance;
	attackDamage_ = attackDamage;

	pAttackCollider_ = this->CreateCollider<SphereCollider>(radius);

	Vector3 forward = { sinf(moveAngle_),0.0f,cosf(moveAngle_) };
	Vector3 offset = pos_ + forward * attackColliderDistance_ + Vector3{ 0.0f, colliderHeight_ * Game::kHalf, 0.0f };

	pAttackCollider_->SetPos(offset);
}

void EnemyBase::CreateRangedAttackCollider(float radius, float speed, int attackDamage)
{
	//攻撃コライダーが既に存在する場合は何もしない
	if (pAttackCollider_)return;

	attackDamage_ = attackDamage;
	isRangedAttack_ = true;
	rangedAttackSpeed_ = speed;

	//攻撃開始時に向いている方向へ進む
	rangedAttackDir_ = { sinf(moveAngle_), 0.0f, -cosf(moveAngle_) };

	pAttackCollider_ = this->CreateCollider<SphereCollider>(radius);

	//生成位置
	Vector3 spawnPos = pos_ + Vector3{ 0.0f, colliderHeight_ * Game::kHalf, 0.0f };
	pAttackCollider_->SetPos(spawnPos);
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
	isRangedAttack_ = false;
}

void EnemyBase::ApplyData(const EnemyData& data, const EnemySpawnData& spawnData, const WayPointLoader* pWayPointLoader)
{
	//経路探索を行う敵の場合
	//Waypointのloaderと
	//エリアIDを設定
	if (pNavigation_)
	{
		pNavigation_->SetWayPointLoader(pWayPointLoader);
		pNavigation_->SetAreaId(spawnData.areaId_);
	}

	//ステータス
	hp_ = spawnData.hp_;

	//最大体力
	maxHP_ = spawnData.hp_;

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
	animation_.RegisterAnimName(AnimationState::BossLeftWalk, data.leftWalkAnim_);
	animation_.RegisterAnimName(AnimationState::BossRightWalk, data.rightWalkAnim_);
	animation_.RegisterAnimName(AnimationState::BossRush, data.bossRushAnim_);
	animation_.RegisterAnimName(AnimationState::Run, data.runAnim_);
	animation_.RegisterAnimName(AnimationState::EnemyAttack, data.attackAnim_);
	animation_.RegisterAnimName(AnimationState::EnemyJumpAttack, data.jumpAttackAnim_);
	animation_.RegisterAnimName(AnimationState::Damage, data.damageAnim_);
	animation_.RegisterAnimName(AnimationState::Death, data.deathAnim_);
	animation_.RegisterAnimName(AnimationState::React, data.reactAnim_);
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
	//経路探索をしない敵は何もしない
	if (!pNavigation_)return;

	pNavigation_->SetNavigationGrid(pNavGrid);

}

void EnemyBase::PlayPrevEnemyAttackEffect()
{
	Vector3 forward = { sinf(moveAngle_), 0.0f, -cosf(moveAngle_) };
	Vector3 effectPos = pos_ + Vector3{ 0.0f, colliderHeight_ * 1.3f, 0.0f } + forward * 60.0f;

	EffectManager::GetInstance().Play(L"EnemyAttack", effectPos);
}

void EnemyBase::SetSlowAnimationSpeed()
{
	animation_.SetSlowAnimationSpeed();
}

void EnemyBase::SetAttackAnimationSpeed()
{
	animation_.SetEnemyAttackAnimationSpeed();
}

bool EnemyBase::IsPlayerInRange(float radius) const
{
	Vector3 toPlayer = pos_ - GetPlayerPos();

	toPlayer.y_ = 0.0f;
	return toPlayer.Length() <= radius;

}

bool EnemyBase::IsPlayerInVision(float maxDist, float visionAngle) const
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return false;

	//プレイヤーから敵のベクトルを計算
	Vector3 playerPos = pPlayer->GetPos();
	Vector3 toPlayer = playerPos - pos_;
	toPlayer.y_ = 0.0f;//高さは使わないので0に

	//距離の判定
	float distSq = toPlayer.LengthSq();

	//最大範囲より外側にいる場合は見えていないからfalseを返す
	if (distSq > maxDist * maxDist)
	{
		return false;
	}

	//0除算を防止するため重なっているときは見えているのでtrueを返す
	if (distSq < 0.0001f) return true;

	float dist = sqrtf(distSq);
	Vector3 dirToPlayer = { toPlayer.x_ / dist, 0.0f, toPlayer.z_ / dist };

	//正面ベクトル
	Vector3 forward = { sinf(moveAngle_), 0.0f, -cosf(moveAngle_) };

	//正面ベクトルとターゲットの方向ベクトルとの内積を計算
	float dot = forward.Dot(dirToPlayer);

	//視野角の半分をラジアンに変換
	float halfFovRad = (visionAngle * Game::kHalf) * kRadian;

	//視野角の半分のcosの値を求める
	float cosHalfFov = cosf(halfFovRad);

	//内積が視野角の半分のcosの値以上であれば視野に入っている
	if (dot >= cosHalfFov)
	{
		return true;
	}

	return false;
}

bool EnemyBase::IsReacting() const
{
	return std::dynamic_pointer_cast<EnemyStateReact>(pCurrentState_) != nullptr;
}

void EnemyBase::DrawDebugSearchRange(const Vector3& centerPos, float radius, unsigned int color)
{
	VECTOR pos = VGet(centerPos.x_, centerPos.y_, centerPos.z_);

	DrawSphere3D(pos, radius, kDrawDebugRangeDiv, color, color, false);
}

void EnemyBase::CreateNavigation()
{
	if (!pNavigation_)
	{
		pNavigation_ = std::make_unique<EnemyNavigation>();
	}
}
