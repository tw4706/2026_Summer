#pragma once
#include "EnemyAttackSubStateBase.h"
#include<memory>

class EnemyAttackSubStatePO :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStatePO(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack, const AttackData& attackData);
	~EnemyAttackSubStatePO() = default;

	virtual void Enter()override;
	virtual void Update()override;
	virtual void Exit()override;
};

