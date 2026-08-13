#pragma once
#include "BossStateBase.h"
class BossStateRangedAttack :public BossStateBase
{
public:
	BossStateRangedAttack(std::weak_ptr<Boss> pBoss, float searchRadius);

	void Enter() override;
	void Update() override;
	void Exit() override;

private:
	float attackTime_ = 0.0f;
};

