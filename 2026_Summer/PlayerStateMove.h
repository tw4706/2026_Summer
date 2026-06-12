#pragma once
#include "PlayerStateBase.h"
class PlayerStateMove :public PlayerStateBase
{
public:
	PlayerStateMove(std::weak_ptr<Player>pPlayer);
	~PlayerStateMove();

	void Init()override;
	void Update()override;
	void End()override;

private:
};

