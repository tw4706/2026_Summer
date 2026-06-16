#pragma once
#include "PlayerStateBase.h"
class Player;
class PlayerStateIdle :public PlayerStateBase
{
public:
    PlayerStateIdle(Player* pPlayer, Input& input, CameraBase& camera);
    ~PlayerStateIdle() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};

