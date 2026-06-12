#pragma once
#include "PlayerStateBase.h"

class PlayerStateIdle :public PlayerStateBase
{
public:
	PlayerStateIdle(std::weak_ptr<Player>pPlayer);
	~PlayerStateIdle();

	void Init()override;
	void Update()override;
	void End()override;

private:
};

