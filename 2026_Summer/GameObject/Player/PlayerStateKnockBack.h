#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateKnockBack :public PlayerStateBase
{
public:
    PlayerStateKnockBack(std::weak_ptr<Player> pPlayer);
    ~PlayerStateKnockBack() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};

