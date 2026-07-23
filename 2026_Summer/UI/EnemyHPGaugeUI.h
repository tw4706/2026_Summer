#pragma once
#include "HPGaugeUI.h"
#include<memory>

class EnemyBase;
class EnemyHPGaugeUI :public HPGaugeUI
{
public:
	EnemyHPGaugeUI(std::weak_ptr<EnemyBase> pEnemy);
	virtual~EnemyHPGaugeUI() = default;

	virtual void Init()override;

	void VisbleDamage();

protected:
	virtual const wchar_t* GetHPGraphPath() const override;
	virtual void UpdateHP() override;
	virtual bool VisibleDrawPos(int& drawX, int& drawY)const override;

protected:
	std::weak_ptr<EnemyBase>pEnemy_;//ìGÇÃé„éQè∆

	bool isVisible_;
	float visibleTimer_;
};

