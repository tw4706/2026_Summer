#pragma once
#include "EnemyAttackSubStateBase.h"
class EnemyAttackSubStateChance :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStateChance(EnemyBase* pEnemy, EnemyStateAttack* pEnemyAttack);
	virtual~EnemyAttackSubStateChance() = default;

	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
};

