#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateAttack :public PlayerStateBase
{
public:
    PlayerStateAttack(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera);
    ~PlayerStateAttack() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};

