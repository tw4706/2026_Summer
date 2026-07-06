#pragma once
#include "PlayerStateBase.h"
class PlayerStateDeath :public PlayerStateBase
{
public:
	PlayerStateDeath(std::weak_ptr<Player> pPlayer, PlayerCamera& camera);
	~PlayerStateDeath() = default;

	void Enter()  override;
	void Update() override;
	void Exit()   override;
};

