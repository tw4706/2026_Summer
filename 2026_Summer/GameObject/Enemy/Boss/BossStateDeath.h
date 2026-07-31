#pragma once
#include "BossStateBase.h"
class BossStateDeath :public BossStateBase
{
public:
	BossStateDeath(std::weak_ptr<Boss>pBoss, float searchRadius);
	~BossStateDeath() = default;

	void Enter()override;
	void Update()override;
	void Exit()override;
};

