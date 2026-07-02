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

    /// <summary>
    /// カメラの回転
    /// </summary>
    /// <param name="yaw">ヨー角</param>
    /// <param name="pitch">ピッチ角</param>
    void AddRotation(float yaw, float pitch)override;

    /// <summary>
    /// プレイヤーの設定
    /// </summary>
    /// <param name="player">プレイヤーのポインタ</param>
    void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }

    /// <summary>
    /// ヨー角の取得
    /// </summary>
    /// <returns>ヨー角</returns>
    virtual float GetYaw() const override { return yaw_; }

private:
    std::shared_ptr<Player> pPlayer_;
    float yaw_;             //ヨー角
    float pitch_;           //ピッチ角
    float currentYaw_;      //現在のヨー角
    float currentPitch_;    //現在のピッチ角
};

