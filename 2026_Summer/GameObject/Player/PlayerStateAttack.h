#pragma once
#include "PlayerStateBase.h"

class Player;
class PlayerStateAttack :public PlayerStateBase
{
public:
    PlayerStateAttack(std::weak_ptr<Player> pPlayer);
    ~PlayerStateAttack() = default;

    void Enter()  override;
    void Update() override;
    void Exit()   override;

private:
    bool isEffectPlaying_ = false;//エフェクトを再生中かどうか
    bool isColliderEnabled_ = false;//コライダーが有効かどうか
};

