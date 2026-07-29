#pragma once
#include "BossStateBase.h"
class BossStateIdle :
public BossStateBase
{
public:
    BossStateIdle(std::weak_ptr<Boss>pBoss,float searchRadius);
    ~BossStateIdle()=default;

    void Enter()override;
    void Update()override;
    void Exit()override;
};

