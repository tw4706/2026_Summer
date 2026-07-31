#include "Boss.h"
#include "BossStateIdle.h"
#include "BossStateDamage.h"
#include "BossStateDeath.h"
#include "Katana.h"
#include "EffectManager.h"
#include<memory>

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//攻撃のクールタイム
	constexpr float kAttackCoolTime = 3.0f;

	//パーセンテージのマックス
	constexpr int kRandMax = 100;

	//ダメージに行く確率
	constexpr int kRandomToDamage = 20;
}

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Init()
{
	//敵の基底クラスの初期化
	EnemyBase::Init();
}

void Boss::Update()
{
	//攻撃のクールタイムの減少
	if (attackCoolTime_ >= 0.0f)
	{
		attackCoolTime_ -= kDeltaTime;
	}

	//ステートがない場合
	if (!pCurrentState_)
	{
		//ボスとして弱参照を作成
		auto boss = std::dynamic_pointer_cast<Boss>(shared_from_this());
		ChangeState(std::make_shared<BossStateIdle>(boss, searchRadius_));
	}

	//敵の基底クラスの更新
	EnemyBase::Update();
}

bool Boss::IsAttackReady() const
{
	return attackCoolTime_ <= 0.0f;
}

void Boss::AttackCoolDown()
{
	attackCoolTime_ = kAttackCoolTime;
}

void Boss::OnDamage(int damage)
{
	hp_ -= damage;

	//被弾したらHPUIを出現させる
	if (auto pGauge = pHPGaugeUI_.lock())
	{
		pGauge->VisbleDamage();
	}

	Vector3 effectPos = pos_ + Vector3{ 0.0f, 70.0f, 0.0f };

	EffectManager::GetInstance().Play(L"Hit", effectPos);

	if (hp_ <= 0)
	{
		hp_ = 0;

		auto boss = std::dynamic_pointer_cast<Boss>(shared_from_this());

		//Boss専用の死亡状態に遷移する
		auto nextState = std::make_shared<BossStateDeath>(boss, searchRadius_);
		ChangeState(nextState);
		return;
	}
}

void Boss::OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)
{
	if (Katana* pKatana = dynamic_cast<Katana*>(&coll))
	{
		if (!pKatana->IsEnabled())
		{
			return;
		}

		//すでにダメージ状態なら何もしない
		if (std::dynamic_pointer_cast<BossStateDamage>(pCurrentState_))
		{
			return;
		}

		//すでに死亡状態なら何もしない
		if (std::dynamic_pointer_cast<BossStateDeath>(pCurrentState_))
		{
			return;
		}

		auto boss = std::dynamic_pointer_cast<Boss>(shared_from_this());

		int rand = std::rand() % kRandMax;

		if (rand < kRandomToDamage)
		{
			auto nextState = std::make_shared<BossStateDamage>(boss, searchRadius_);
			ChangeState(nextState);
		}
	}
	else
	{
		isHit_ = true;
	}
}
