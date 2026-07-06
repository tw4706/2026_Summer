#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"

namespace
{
    //Attack
    const wchar_t* kPlayerAttack = L"Player|Attack";
}

PlayerStateAttack::PlayerStateAttack(std::weak_ptr<Player> pPlayer, PlayerCamera& camera) :
	PlayerStateBase(pPlayer,camera)
{
}

void PlayerStateAttack::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    //攻撃アニメーションに遷移
    pPlayer->ChangeAnimation(AnimationState::Attack, kPlayerAttack);

    //攻撃開始時に移動速度を0にする
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });

    //攻撃開始と同時に刀の当たり判定を有効化
    pPlayer->SetKatanaColliderEnabled(true);
}

void PlayerStateAttack::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    //攻撃中は移動入力を一切受け付けず、速度もゼロを維持する
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });

    //アニメーションが終了したらIdle状態へ戻る
    if (pPlayer->IsAnimationEnd())
    {
        //刀の当たり判定を無効化
        pPlayer->SetKatanaColliderEnabled(false);
        pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, camera_));
        return;
    }
}

void PlayerStateAttack::Exit()
{
    //念のためExitでも無効化
    auto pPlayer = pPlayer_.lock();
    if (pPlayer)
    {
        pPlayer->SetKatanaColliderEnabled(false);
    }
}
