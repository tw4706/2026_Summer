#pragma once
#include "CSV/EnemyAttackData.h"
#include<memory>

class EnemyBase;
class EnemyStateAttack;
class EnemyAttackSubStateBase
{
public:

	//コンストラクタ
	EnemyAttackSubStateBase(std::weak_ptr<EnemyBase>pEnemy, EnemyStateAttack* pEnemyAttack);

	//コンストラクタ(オーバーロード)
	EnemyAttackSubStateBase(std::weak_ptr<EnemyBase>pEnemy, EnemyStateAttack* pEnemyAttack, const AttackData& attackData);
	virtual~EnemyAttackSubStateBase() = default;

	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;

protected:
	std::weak_ptr<EnemyBase> pEnemy_;		//敵ベースのポインタ
	EnemyStateAttack* pEnemyAttack_ = nullptr;//敵の攻撃ステートのポインタ

	AttackData attackData_;//攻撃のデータ
};

