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
    virtual void Update(int stageModelHandle)override;

    /// <summary>
    /// カメラの回転
    /// </summary>
    /// <param name="yaw">ヨー角</param>
    /// <param name="pitch">ピッチ角</param>
    void AddRotation(float yaw, float pitch)override;

    /// <summary>
    /// 回転のセット
    /// </summary>
    /// <param name="yaw">ヨー角</param>
    /// <param name="pitch"ピッチ角></param>
    void SetRotation(float yaw, float pitch);

    /// <summary>
    /// 回転角度の設定
    /// </summary>
    /// <param name="yaw">ヨー角</param>
    /// <param name="pitch">ピッチ角</param>
    void SetRotationToLockOn(const Vector3& lockOnPos, const Vector3& lockOnTarget);

    /// <summary>
    /// プレイヤーの設定
    /// </summary>
    /// <param name="player">プレイヤーのポインタ</param>
    void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }
private:
    std::shared_ptr<Player> pPlayer_;
};

