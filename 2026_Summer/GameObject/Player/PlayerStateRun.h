#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateRun :public PlayerStateBase
{
public:
    PlayerStateRun(std::weak_ptr<Player> pPlayer);
    ~PlayerStateRun() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};