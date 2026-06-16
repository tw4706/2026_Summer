#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateAttack :public PlayerStateBase
{
public:
    PlayerStateAttack(Player* pPlayer, Input& input, CameraBase& camera);
    ~PlayerStateAttack() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};

