#pragma once
#include "EnemyAttackSubStateBase.h"
#include<memory>
class EnemyAttackSubStateChance :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStateChance(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack);
	~EnemyAttackSubStateChance() = default;

	virtual void Enter() override;
	virtual void Update() override;
	virtual void Exit() override;
};

