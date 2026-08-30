#pragma once
#include "EnemyHPGaugeUI.h"
class BossHPGaugeUI :public EnemyHPGaugeUI
{
public:
	BossHPGaugeUI(std::weak_ptr<EnemyBase> pEnemy);
	virtual ~BossHPGaugeUI() = default;

	virtual void Init() override;

protected:
	virtual bool VisibleDrawPos(int& drawX, int& drawY) const override;
};

