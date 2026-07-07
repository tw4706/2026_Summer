#pragma once
#include "PlayerStateBase.h"

class PlayerStateGuard :public PlayerStateBase
{
public:
	PlayerStateGuard(std::weak_ptr<Player> pPlayer, CameraBase& camera);
	~PlayerStateGuard() = default;

	void Enter()  override;
	void Update() override;
	void Exit()   override;
};

