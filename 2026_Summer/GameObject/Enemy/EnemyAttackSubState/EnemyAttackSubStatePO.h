#pragma once
#include "EnemyAttackSubStateBase.h"
class EnemyAttackSubStatePO :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStatePO(EnemyBase* pEnemy, EnemyStateAttack* pEnemyAttack);
	~EnemyAttackSubStatePO() = default;

	virtual void Enter()override;
	virtual void Update()override;
	virtual void Exit()override;
};

