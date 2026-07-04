#pragma once
#include "EnemyAttackSubStateBase.h"
class EnemyAttackSubStateAttack :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStateAttack(EnemyBase* pEnemy, EnemyStateAttack* pEnemyAttack);
	virtual~EnemyAttackSubStateAttack() = default;

	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
};

