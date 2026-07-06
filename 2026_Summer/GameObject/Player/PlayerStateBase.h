#pragma once
#include "CharacterStateBase.h"

class Player;
class PlayerCamera;
class Vector3;
class PlayerStateBase :public CharacterStateBase
{
public:
    PlayerStateBase(std::weak_ptr<Player> pPlayer, PlayerCamera& camera);
    virtual ~PlayerStateBase() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

protected:
    Vector3 GetCameraLookMoveDirection()const;

protected:
    std::weak_ptr<Player> pPlayer_;
    PlayerCamera& camera_;
};

