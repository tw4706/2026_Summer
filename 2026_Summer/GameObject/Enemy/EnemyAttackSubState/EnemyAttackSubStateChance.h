#pragma once
#include "EnemyAttackSubStateBase.h"
class EnemyAttackSubStateChance :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStateChance(EnemyBase* pEnemy, EnemyStateAttack* pEnemyAttack);
	~EnemyAttackSubStateChance() = default;

	virtual void Enter() override;
	virtual void Update() override;
	virtual void Exit() override;
};

