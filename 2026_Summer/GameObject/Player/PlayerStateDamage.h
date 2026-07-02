#pragma once
#include "PlayerStateBase.h"
class PlayerStateDamage :public PlayerStateBase
{
public:
	PlayerStateDamage(std::weak_ptr<Player> pPlayer, Input& input, PlayerCamera& camera);
	~PlayerStateDamage() = default;

	void Enter()  override;
	void Update() override;
	void Exit()   override;
};

