#pragma once

class EnemyBase;
class EnemyStateAttack;
class EnemyAttackSubStateBase
{
public:
	EnemyAttackSubStateBase(EnemyBase*pEnemy,EnemyStateAttack*pEnemyAttack);
	virtual~EnemyAttackSubStateBase()=default;

	virtual void Enter()=0;
	virtual void Update() = 0;
	virtual void Exit()=0;

protected:
	EnemyBase* pEnemy_=nullptr;//敵ベースのポインタ
	EnemyStateAttack* pEnemyAttack_=nullptr;//敵の攻撃ステートのポインタ
};

