#pragma once
#include "EnemyStateBase.h"

class EnemyStateIdle :public EnemyStateBase
{
public:
	EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy);
	~EnemyStateIdle() = default;

	void Enter()  override;
	void Update() override;
	void Exit()   override;
};

