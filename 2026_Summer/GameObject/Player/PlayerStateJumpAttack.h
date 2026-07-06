#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateJumpAttack :public PlayerStateBase
{
public:
    PlayerStateJumpAttack(std::weak_ptr<Player> pPlayer, PlayerCamera& camera);
    ~PlayerStateJumpAttack() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};

