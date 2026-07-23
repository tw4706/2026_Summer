#pragma once
#include "HPGaugeUI.h"
#include<memory>

class Player;
class PlayerHPGaugeUI :public HPGaugeUI
{
public:
	PlayerHPGaugeUI(std::weak_ptr<Player> pPlayer);
	virtual~PlayerHPGaugeUI();

	virtual void Init()override;

protected:	
	virtual const wchar_t* GetHPGraphPath() const override;
	virtual void UpdateHP() override;
	virtual bool VisibleDrawPos(int& drawX, int& drawY)const override;

protected:
	std::weak_ptr<Player>pPlayer_;//プレイヤーの弱参照
};

