#pragma once
#include "EnemyAttackSubStateBase.h"
class EnemyAttackSubStatePO :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStatePO(EnemyBase* pEnemy, EnemyStateAttack* pEnemyAttack);
	virtual~EnemyAttackSubStatePO() = default;

	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
};

