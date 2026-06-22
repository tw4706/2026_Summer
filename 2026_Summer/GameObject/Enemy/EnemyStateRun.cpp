#include "EnemyStateRun.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"
#include<cmath>

namespace
{
    //敵の走りアニメーション
    const wchar_t* kEnemyRun = L"Oni|Run";

    const float kMoveSpeed = 0.3f;

    //線形補間の割合
    const float kRotateLerpRate = 0.1f;
}

EnemyStateRun::EnemyStateRun(std::weak_ptr<EnemyBase> pEnemy) :
    EnemyStateBase(pEnemy)
{
}

void EnemyStateRun::Enter()
{
    auto pEnemy = pEnemy_.lock();
    if (!pEnemy) return;

    //Runアニメーションに遷移
    pEnemy->ChangeAnimation(AnimationState::Run, kEnemyRun);
}

void EnemyStateRun::Update()
{
    auto enemy = pEnemy_.lock();
    if (!enemy) return;

    if (PlayerSearchDistance(200.0f)==false)
    {
        //検知範囲に入ったらRun状態へ遷移
        auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_);
        enemy->ChangeState(nextState);
        return;
    }

    //敵とプレイヤーの位置を取得
    Vector3 enemyPos = enemy->GetPos();
    Vector3 playerPos = enemy->GetPlayerPos();

    //敵からプレイヤーまでのベクトルを計算
    Vector3 toPlayer = playerPos - enemyPos;
    toPlayer.y_ = 0.0f;

    //距離の計算
    float distance = std::sqrt(toPlayer.x_ * toPlayer.x_ + toPlayer.z_ * toPlayer.z_);
    if (distance < 0.1f) return;

    toPlayer.Normalize();

    const float kDeltaTime = 1.0f / 60.0f;
    Vector3 velocity = { toPlayer.x_ * kMoveSpeed * kDeltaTime, 0.0f, toPlayer.z_ * kMoveSpeed * kDeltaTime };

    //計算した位置を適応
    Vector3 nextPos = enemyPos + velocity;
    enemy->SetPos(nextPos);

    float targetAngle= std::atan2f(toPlayer.x_, -toPlayer.z_);

    float currentAngle = enemy->GetMoveAngle();

    float nextAngle = currentAngle + (targetAngle - currentAngle) * kRotateLerpRate;

    // 計算した角度を適用
    enemy->SetMoveAngle(nextAngle);
}

void EnemyStateRun::Exit()
{
}
