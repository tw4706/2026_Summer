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

protected:
    std::weak_ptr<EnemyBase> pEnemy_;
};

