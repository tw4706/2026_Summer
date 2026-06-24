#pragma once
#include "EnemyStateBase.h"
class EnemyStateDeath :public EnemyStateBase
{
public:
	EnemyStateDeath(std::weak_ptr<EnemyBase> pEnemy);
	~EnemyStateDeath() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
};

