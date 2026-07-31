#include "Boss.h"
#include "BossStateIdle.h"
#include<memory>

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//攻撃のクールタイム
	constexpr float kAttackCoolTime = 3.0f;
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
