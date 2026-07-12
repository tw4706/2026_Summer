#pragma once
#include "EnemyStateBase.h"
class EnemyStateGuard :public EnemyStateBase
{
public:
	EnemyStateGuard(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateGuard() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
};

