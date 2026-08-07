#pragma once
#include "CameraBase.h"

class CameraManager;
class Player;
class EnemyBase;
class BossCamera :public CameraBase
{
public:
    BossCamera();
    void Init() override;
    void Update(int stageModelHandle) override;

    /// <summary>
    /// 注視点の設定
    /// </summary>
    /// <param name="pPlayer">対象になるプレイヤー</param>
    /// <param name="pBoss">対象となるボス</param>
    void SetTarget(std::weak_ptr<Player> pPlayer, std::weak_ptr<EnemyBase> pBoss, CameraManager* pCameraManager);

private:
    std::weak_ptr<Player> pPlayer_;
    std::weak_ptr<EnemyBase> pBoss_;
    CameraManager* pCameraManager_ = nullptr;

    float bossEventTimer_ = 0.0f;//ボスイベントのタイマー
};

