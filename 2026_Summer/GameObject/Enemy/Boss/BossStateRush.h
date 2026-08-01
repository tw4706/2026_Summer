#pragma once
#include "BossStateBase.h"
#include"Math/Vector3.h"

class BossStateRush :public BossStateBase
{
public:
	BossStateRush(std::weak_ptr<Boss> pBoss, float searchRadius);
	~BossStateRush()=default;

	void Enter() override;
	void Update() override;
	void Exit() override;

private:
	//突進する方向
	Vector3 dashDir_;
	//突進開始からの時間
	float dashTime_;
};

