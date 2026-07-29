#pragma once
#include "BossStateBase.h"
class BossStateRun :public BossStateBase
{
public:
	BossStateRun(std::weak_ptr<Boss>pBoss, float searchRadius);
	~BossStateRun() = default;

	void Enter()override;
	void Update()override;
	void Exit()override;
};

