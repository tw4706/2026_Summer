#pragma once
#include "EnemyStateBase.h"
#include "EnemyAttackSubState/EnemyAttackSubStateBase.h"
#include<memory>

class EnemyStateAttack :public EnemyStateBase
{
public:
	EnemyStateAttack(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateAttack() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;

	/// <summary>
	/// UŒ‚ó‘Ô‚Ì‘JˆÚ
	/// </summary>
	/// <param name="nextState">Ÿ‚ÌUŒ‚‚Ìó‘Ô</param>
	void ChangeAttackState(std::shared_ptr<EnemyAttackSubStateBase>nextAttackState);

private:
	//Œ»İ‚ÌUŒ‚‚ÌƒTƒuƒXƒe[ƒg
	std::shared_ptr<EnemyAttackSubStateBase>pCurrentAttackState_ = nullptr;
};

