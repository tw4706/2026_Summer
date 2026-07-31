#pragma once
#include "Enemy/EnemyStateBase.h"
#include<memory>

class Boss;
class BossStateBase :public EnemyStateBase
{
public:
    BossStateBase(std::weak_ptr<Boss>pBoss,float searchRadius);
    virtual ~BossStateBase()=default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;
protected:
    std::weak_ptr<Boss> pBoss_;
};

