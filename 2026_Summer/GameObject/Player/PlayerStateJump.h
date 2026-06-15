#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateJump :public PlayerStateBase
{
public:
    PlayerStateJump(std::weak_ptr<Player> pPlayer, Input& input, CameraBase& camera);
    ~PlayerStateJump() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};

