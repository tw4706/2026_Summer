#pragma once
#include "CharacterStateBase.h"

class Player;
class CameraBase;
class Vector3;
class PlayerStateBase :public CharacterStateBase
{
public:
    PlayerStateBase(std::weak_ptr<Player> pPlayer);
    virtual ~PlayerStateBase() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

protected:
    /// <summary>
    /// カメラの見ている位置と向きを取得する関数
    /// </summary>
    /// <returns>カメラの向きを返す</returns>
    Vector3 GetCameraLookMoveDirection()const;

    /// <summary>
    /// アクティブなカメラの取得
    /// </summary>
    /// <returns></returns>
    CameraBase* GetActiveCamera() const;
protected:

    std::weak_ptr<Player> pPlayer_; //プレイヤーの弱参照
};

