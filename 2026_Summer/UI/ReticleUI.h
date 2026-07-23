#pragma once
#include "UIBase.h"
#include<memory>

class Player;
class ReticleUI :public UIBase
{
public:
	ReticleUI(std::weak_ptr<Player> pPlayer);
	virtual ~ReticleUI();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	std::weak_ptr<Player> pPlayer_; //プレイヤーの弱参照

	int reticleHandle_;
	int reticleX_;
	int reticleY_;
};

