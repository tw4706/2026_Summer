#pragma once
#include "CharacterStateBase.h"

class Player;
class CameraBase;
class Vector3;
class PlayerStateBase :public CharacterStateBase
{
public:
    PlayerStateBase(std::weak_ptr<Player> pPlayer, CameraBase& camera);
    virtual ~PlayerStateBase() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

protected:
    /// <summary>
    /// ƒJƒƒ‰‚ÌŒ©‚Ä‚¢‚éˆÊ’u‚ÆŒü‚«‚ğæ“¾‚·‚éŠÖ”
    /// </summary>
    /// <returns>ƒJƒƒ‰‚ÌŒü‚«‚ğ•Ô‚·</returns>
    Vector3 GetCameraLookMoveDirection()const;
protected:
    std::weak_ptr<Player> pPlayer_;
    CameraBase& pCamera_;
};

