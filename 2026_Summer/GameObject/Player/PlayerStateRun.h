#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateRun :public PlayerStateBase
{
public:
    PlayerStateRun(Player* pPlayer, Input& input, CameraBase& camera);
    ~PlayerStateRun() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;
};