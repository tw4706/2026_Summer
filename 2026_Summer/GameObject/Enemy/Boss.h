#pragma once
#include "EnemyBase.h"
class Boss :public EnemyBase
{
public:
	Boss();
	virtual~Boss();

	virtual void Init()override;
	virtual void Update()override;

	/// <summary>
	/// 攻撃の準備ができたかどうか
	/// </summary>
	/// <returns></returns>
	bool IsAttackReady()const;

	/// <summary>
	/// 攻撃のクールダウン
	/// </summary>
	void AttackCoolDown();

private:
	float attackCoolTime_ = 0.0f;//攻撃のクールタイム
};

