#pragma once
#include "EnemyStateBase.h"
class EnemyStateRun :public EnemyStateBase
{
public:
	EnemyStateRun(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateRun() = default;

	void Enter()override;
	void Update()override;
	void Exit()override;

private:
	float loseSightTimer_ = 0.0f;//見失っている時間を図るタイマー
};

