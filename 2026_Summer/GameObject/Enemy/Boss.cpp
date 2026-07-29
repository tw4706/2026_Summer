#include "Boss.h"
#include "EnemyStateIdle.h"
#include<memory>

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;
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
	//ステートがない場合
	if (!pCurrentState_)
	{
		//ボスとして弱参照を作成
		auto boss = std::dynamic_pointer_cast<Boss>(shared_from_this());
		ChangeState(std::make_shared<EnemyStateIdle>(boss, searchRadius_));
	}

	//敵の基底クラスの更新
	EnemyBase::Update();
}
