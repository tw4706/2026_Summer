#include "EnemyStateRun.h"
#include "EnemyBase.h"

namespace
{
    //敵の走りアニメーション
    const wchar_t* kEnemyRun = L"Oni|Walk";

    //線形補間の割合
    const float kLerpRate = 0.05f;
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

    Vector3 enemyPos = enemy->GetPos();
    Vector3 playerPos = enemy->GetPlayerPos();

    Vector3 nextPos;
    nextPos.x_ = enemyPos.x_ + (playerPos.x_ - enemyPos.x_) * kLerpRate;
    nextPos.y_ = enemyPos.y_ + (playerPos.y_ - enemyPos.y_) * kLerpRate;
    nextPos.z_ = enemyPos.z_ + (playerPos.z_ - enemyPos.z_) * kLerpRate;

    //計算した位置を適応
    enemy->SetPos(nextPos);
}

void EnemyStateRun::Exit()
{
}
