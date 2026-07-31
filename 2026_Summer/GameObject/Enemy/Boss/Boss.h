#pragma once
#include "Enemy/EnemyBase.h"
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

	/// <summary>
	/// ダー目地処理
	/// </summary>
	/// <param name="damage">ダメージ</param>
	void OnDamage(int damage)override;

	void OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)override;

private:
	float attackCoolTime_ = 0.0f;//攻撃のクールタイム
};

