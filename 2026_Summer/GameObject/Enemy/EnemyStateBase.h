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
    /// 視線が通っているか判定(hitしていなければtrue)
    /// </summary>
    /// <param name="stageModelHandle">ステージモデルのハンドル</param>
    /// <param name="start">視線の始点</param>
    /// <param name="end">視線の終点</param>
    /// <param name="eyeHeight">レイを飛ばす高さのオフセット</param>
    static bool HasLineOfSight(int stageModelHandle, const Vector3& start, const Vector3& end, float eyeHeight = 50.0f);

    /// <summary>
    /// fromからtoへ直進した場合に歩行不可のマスが無いか一定間隔でチェックする
    /// </summary>
    /// <param name="pNaviGrid">ナビゲーショングリッド</param>
    /// <param name="start">始点</param>
    /// <param name="end">終点</param>
    static bool IsPathWalkable(const NavigationGrid* pNaviGrid, const Vector3& start, const Vector3& end);

    /// <summary>
    /// 目標方向を向くよう角度を線形補間して次のフレームの角度を求める関数
    /// </summary>
    /// <param name="currentAngle">現在の角度</param>
    /// <param name="toTargetDir">目標方向のベクトル</param>
    /// <param name="lerpRate">補間の割合</param>
    static float RotateAngle(float currentAngle, const Vector3& toTargetDir, float lerpRate);

    /// <summary>
    /// 目標方向へ移動速度分だけ進めて速度・位置を敵に反映する関数
    /// </summary>
    /// <param name="enemy">敵の参照</param>
    /// <param name="enemyPos">敵の現在位置</param>
    /// <param name="toTargetDir">目標方向のベクトル</param>
    /// <param name="moveSpeed">移動速度</param>
    static void ApplyMove(const std::shared_ptr<EnemyBase>& enemy, const Vector3& enemyPos, const Vector3& toTargetDir, float moveSpeed);

    /// <summary>
    /// 敵の移動経路を計算する関数
    /// </summary>
    /// <param name="enemy">移動経路を計算する対象の敵のポインタ。</param>
    /// <param name="startPos">開始位置/param>
    /// <param name="endPos">終了位置</param>
    /// <param name="outHasLineOfSight">敵が移動経路上で視線を通ってるかどうか</param>
    /// <returns>計算された移動経路の最終位置</returns>
    static Vector3 MoveTargetPath(const std::shared_ptr<EnemyBase>& enemy, const Vector3& startPos, const Vector3& endPos, bool& outHasLineOfSight);

protected:
    //索敵範囲
    float searchRadius_ = 0.0f;

    //敵の弱参照
    std::weak_ptr<EnemyBase> pEnemy_;
};

