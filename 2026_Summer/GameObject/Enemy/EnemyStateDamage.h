#pragma once
#include "EnemyStateBase.h"
class EnemyStateDamage :public EnemyStateBase
{
public:
	EnemyStateDamage(std::weak_ptr<EnemyBase> pEnemy);
	~EnemyStateDamage() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
};

