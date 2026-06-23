#pragma once
#include "EnemyStateBase.h"
class EnemyStateAttack :public EnemyStateBase
{
public:
	EnemyStateAttack(std::weak_ptr<EnemyBase> pEnemy);
	~EnemyStateAttack() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
};

