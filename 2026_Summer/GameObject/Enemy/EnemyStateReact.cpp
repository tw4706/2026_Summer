#include "EnemyStateReact.h"
#include "EnemyStateIdle.h"
#include "EnemyStateRun.h"
#include "EnemyBase.h"

namespace
{
	//見渡す角度
	constexpr float kAngle = DX_PI_F / 6.0f;

	//見渡しているフレーム数
	constexpr float kAngleFrame = 180.0f;
}

EnemyStateReact::EnemyStateReact(std::weak_ptr<EnemyBase> pEnemy, float searchRadius):
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateReact::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::React);

	//敵からプレイヤーへのベクトルを取り敵の向く角度を計算
	Vector3 toPlayer = enemy->GetPlayerPos() - enemy->GetPos();
	toPlayer.y_ = 0.0f;
	enemy->moveAngle_ = atan2f(toPlayer.x_, -toPlayer.z_);
}

void EnemyStateReact::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//タイマーの更新
	moveAngleFrame_++;

	bool isPlayerInRange = enemy->IsPlayerInRange(searchRadius_);

	//プレイヤーが範囲に入ったら
	if (isPlayerInRange)
	{
		//障害物越しには気づかれないようにしたいので敵の視線の判定を行う
		Vector3 enemyPos = enemy->GetPos();
		Vector3 playerPos = enemy->GetPlayerPos();
		bool hasLineOfSight = HasLineOfSight(enemy->GetStageModelHandle(), enemyPos, playerPos);

		if (hasLineOfSight)
		{
			//プレイヤーを発見したら追いかける
			auto nextState = std::make_shared<EnemyStateRun>(pEnemy_, searchRadius_);
			enemy->ChangeState(nextState);
			return;
		}
	}
}

void EnemyStateReact::Exit()
{
}
