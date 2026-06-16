#pragma once
#include "CameraBase.h"
#include<memory>

class Player;
class Input;
class PlayerCamera :public CameraBase
{
public:
    PlayerCamera();
    virtual ~PlayerCamera();

    virtual void Init() override;
    virtual void Update() override {};
    virtual void Update(Input&input);

    /// <summary>
    /// カメラの回転
    /// </summary>
    /// <param name="yaw">ヨー角</param>
    /// <param name="pitch">ピッチ角</param>
    void AddRotation(float yaw, float pitch)override;

    /// <summary>
    /// プレイヤーのセット
    /// </summary>
    /// <param name="player">プレイヤーのスマートポインタ</param>
    void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }

    /// <summary>
    /// ヨー角の取得
    /// </summary>
    /// <returns>ヨー角</returns>
    virtual float GetYaw() const override { return yaw_; }

private:
    float yaw_;
    float pitch_;                       //ピッチ角
    std::shared_ptr<Player> pPlayer_;   //プレイヤー
};

