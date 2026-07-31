#pragma once
#include "BossStateBase.h"
class BossStateDamage :public BossStateBase
{
public:
	BossStateDamage(std::weak_ptr<Boss>pBoss,float searchRadius);
	~BossStateDamage()=default;

	void Enter()override;
	void Update()override;
	void Exit()override;
};