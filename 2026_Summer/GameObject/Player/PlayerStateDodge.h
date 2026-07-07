#pragma once
#include "PlayerStateBase.h"
class Player;
class PlayerStateDodge :public PlayerStateBase
{
public:
	PlayerStateDodge(std::weak_ptr<Player> pPlayer, CameraBase& camera);
	~PlayerStateDodge() = default;

	void Enter()  override;
	void Update() override;
	void Exit()   override;

private:
	float invincibleTimer_;	//無敵時間を測るタイマー
};

