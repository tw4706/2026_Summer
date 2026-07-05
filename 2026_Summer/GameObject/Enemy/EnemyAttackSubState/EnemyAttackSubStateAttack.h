#pragma once
#include "EnemyAttackSubStateBase.h"
class EnemyAttackSubStateAttack :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStateAttack(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack);
	~EnemyAttackSubStateAttack() = default;

	virtual void Enter() override;
	virtual void Update() override;
	virtual void Exit() override;
};

