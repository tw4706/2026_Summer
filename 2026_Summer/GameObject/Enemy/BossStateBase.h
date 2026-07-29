#pragma once
#include "EnemyStateBase.h"
#include<memory>

class BossStateBase :public EnemyStateBase
{
public:
    BossStateBase(std::weak_ptr<EnemyBase>pEnemy,float searchRadius);
    virtual ~BossStateBase()=default;
};

