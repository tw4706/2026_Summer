#pragma once
#include "EnemyStateBase.h"
class EnemyStateRun :public EnemyStateBase
{
public:
	EnemyStateRun(std::weak_ptr<EnemyBase> pEnemy);
	virtual~EnemyStateRun();

	void Enter()override;
	void Update()override;
	void Exit()override;
};

