#pragma once
#include "CharacterStateBase.h"

class Vector3;
class EnemyBase;
class NavigationGrid;
class EnemyStateBase :public CharacterStateBase
{
public:
    EnemyStateBase(std::weak_ptr<EnemyBase> pEnemy,float searchRadius);
    virtual ~EnemyStateBase() = default;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

    /// <summary>
    /// プレイヤーを検知する距離の計算
    /// </summary>
    /// <param name="radius">プレイヤーを検知する半径</param>
    /// <returns></returns>
    bool PlayerSearchDistance(float radius)const;
protected:

    /// <summary>
    /// fromからtoへ視線が通っているか判定する(hitしていなければtrue)
    /// </summary>
    /// <param name="stageModelHandle">ステージモデルのハンドル</param>
    /// <param name="from">視線の始点</param>
    /// <param name="to">視線の終点</param>
    /// <param name="eyeHeight">レイを飛ばす高さのオフセット</param>
    static bool HasLineOfSight(int stageModelHandle, const Vector3& from, const Vector3& to, float eyeHeight = 50.0f);

    /// <summary>
    /// fromからtoへ直進した場合に歩行不可のマスが無いか一定間隔でチェックする
    /// </summary>
    /// <param name="pNaviGrid">ナビゲーショングリッド</param>
    /// <param name="from">始点</param>
    /// <param name="to">終点</param>
    static bool IsPathWalkable(const NavigationGrid* pNaviGrid, const Vector3& from, const Vector3& to);

    /// <summary>
    /// 目標方向を向くよう角度を線形補間して次のフレームの角度を求める
    /// </summary>
    /// <param name="currentAngle">現在の角度</param>
    /// <param name="toTargetDir">目標方向のベクトル(正規化済み)</param>
    /// <param name="lerpRate">補間の割合</param>
    static float RotateAngle(float currentAngle, const Vector3& toTargetDir, float lerpRate);

    /// <summary>
    /// 目標方向へ移動速度分だけ進め、速度・位置を敵に反映する
    /// </summary>
    /// <param name="enemy">敵の参照</param>
    /// <param name="enemyPos">敵の現在位置</param>
    /// <param name="toTargetDir">目標方向のベクトル(正規化済み)</param>
    /// <param name="moveSpeed">移動速度</param>
    static void ApplyMove(const std::shared_ptr<EnemyBase>& enemy, const Vector3& enemyPos, const Vector3& toTargetDir, float moveSpeed);

protected:
    //索敵範囲
    float searchRadius_ = 0.0f;

    //敵の弱参照
    std::weak_ptr<EnemyBase> pEnemy_;
};

