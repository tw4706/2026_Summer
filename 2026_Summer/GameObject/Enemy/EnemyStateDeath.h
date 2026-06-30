#pragma once
#include "EnemyStateBase.h"
class EnemyStateDeath :public EnemyStateBase
{
public:
	EnemyStateDeath(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateDeath() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
};

