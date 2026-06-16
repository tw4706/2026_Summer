#pragma once
#include "CharacterStateBase.h"

class Player;
class Input;
class CameraBase;
class Vector3;
class PlayerStateBase :public CharacterStateBase
{
public:
    PlayerStateBase(Player* pPlayer, Input& input, CameraBase& camera);
    virtual ~PlayerStateBase() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

protected:
    Vector3 GetCameraLookMoveDirection()const;

protected:
    Player* pPlayer_;
    Input& input_;
    CameraBase& camera_;
};

