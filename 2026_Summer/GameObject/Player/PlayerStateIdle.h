#pragma once
#include "PlayerStateBase.h"
class Player;
class PlayerStateIdle :public PlayerStateBase
{
public:
    PlayerStateIdle(std::weak_ptr<Player> pPlayer);
    ~PlayerStateIdle() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};

