#pragma once
#include "CharacterStateBase.h"

class Vector3;
class EnemyBase;
class EnemyStateBase :public CharacterStateBase
{
public:
    EnemyStateBase(std::weak_ptr<EnemyBase> pEnemy);
    virtual ~EnemyStateBase() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

    /// <summary>
    /// プレイヤーを検知する距離の計算
    /// </summary>
    /// <param name="radius">プレイヤーを検知する半径</param>
    /// <returns></returns>
    bool PlayerSearchDistance(float radius)const;

protected:
    std::weak_ptr<EnemyBase> pEnemy_;
};

