#pragma once
#include "CameraBase.h"
#include<memory>

class Player;
class PlayerCamera :public CameraBase
{
public:
    PlayerCamera();
    virtual ~PlayerCamera();

    virtual void Init() override;
    virtual void Update() override;

    // プレイヤーカメラ独自の機能
    void AddRotation(float yaw, float pitch)override;
    void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }
    virtual float GetYaw() const override { return yaw_; }

private:
    std::shared_ptr<Player> pPlayer_;
    float yaw_;
    float pitch_;
};

