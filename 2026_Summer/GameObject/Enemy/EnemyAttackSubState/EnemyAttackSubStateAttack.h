#pragma once
#include "EnemyAttackSubStateBase.h"
class EnemyAttackSubStateAttack :public EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStateAttack(std::weak_ptr<EnemyBase> pEnemy, EnemyStateAttack* pEnemyAttack, const AttackData& attackData);
	~EnemyAttackSubStateAttack() = default;

	virtual void Enter() override;
	virtual void Update() override;
	virtual void Exit() override;

private:
	bool isAttackColliderActive_ = false;//攻撃コライダーがアクティブかどうか
};

