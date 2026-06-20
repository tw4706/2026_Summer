#pragma once
#include "EnemyStateBase.h"

class EnemyStateIdle :public EnemyStateBase
{
public:
	EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy);
	~EnemyStateIdle() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
};

