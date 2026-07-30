#pragma once
#include "BossStateBase.h"
class BossStateAttack :public BossStateBase
{
public:
	BossStateAttack(std::weak_ptr<Boss>pBoss, float searchRadius);
	~BossStateAttack() = default;

	void Enter()override;
	void Update()override;
	void Exit()override;
private:
	float attackTime_=0.0f;
};

