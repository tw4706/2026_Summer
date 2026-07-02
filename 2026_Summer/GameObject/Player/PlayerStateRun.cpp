#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "PlayerStateDodge.h"
#include "Player.h"
#include "Input.h"
#include "Camera/PlayerCamera.h"
#include "Matrix4x4.h"

namespace
{
    //Run
    const wchar_t* kPlayerRun = L"Player|Run";

    //プレイヤーの移動速度
    constexpr float kSpeed = 10.0f;

    //停止する際の線形補間
    constexpr float kStopLerp = 0.2f;

    //カメラの回転速度
    constexpr float kCameraSpeed = 0.03f;

	//カメラのピッチ角
    constexpr float kCameraPitch = 0.3f;

    //スティックの回転の線形補間
    constexpr float kRotateLerpAnalogStick = 0.3f;

    //回転の線形補間
    constexpr float kRotateLerp = 0.3f;

    //移動の入力のしきい値
    constexpr float kRunEpsilon = 0.01f;

    //移動時の線形補間
    constexpr float kMoveLerp = 0.15f;

    //アナログスティックのデッドゾーン
    constexpr float kStickDeadZone = 0.15f;
}

PlayerStateRun::PlayerStateRun(std::weak_ptr<Player> pPlayer, Input& input, PlayerCamera& camera) :
    PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateRun::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    //Runアニメーションに遷移
    pPlayer->ChangeAnimation(AnimationState::Run, kPlayerRun);
}

void PlayerStateRun::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    //プレイヤーの方向ベクトルを取得
    Vector3 playerDir = GetCameraLookMoveDirection();
#ifdef _DEBUG
    DrawFormatString(0, 200, GetColor(255, 255, 255), L"playerDir: %.2f, %.2f, %.2f", playerDir.x_, playerDir.y_, playerDir.z_);
#endif
    //現在の速度・向きの取得
    Vector3 currentVel = pPlayer->GetVelocity();
    float currentAngle = pPlayer->GetMoveAngle();

    //入力がある場合のみ移動・旋回を行う
    if (playerDir.LengthSq() > 0.001f)
    {
        //ターゲットの速度を計算して線形補間で滑らかに加速
        Vector3 targetVel = playerDir * kSpeed;
        currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, kMoveLerp);
        currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, kMoveLerp);

        //進行方向を向く
        float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
        float diff = playerAngle - currentAngle;
        while (diff > DX_PI_F)  diff -= DX_PI_F * 2.0f;
        while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

        currentAngle += diff * kRotateLerp;
    }

    //計算した速度と角度をPlayerに適用し、移動させる
    pPlayer->SetVelocity(currentVel);
    pPlayer->SetMoveAngle(currentAngle);
    pPlayer->AddPosition();

    //カメラの回転処理
    Vector3 stickR = input_.GetStickRight();
    camera_.AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);

    //ジャンプボタンが押されたらジャンプへ
    if (input_.IsTriggered("jump"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    //空中にいる場合はジャンプ状態へ遷移
    if (!pPlayer->GetIsGround())
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    //攻撃ボタンが押されたら攻撃へ遷移
    if (input_.IsTriggered("attack"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, input_, camera_));
        return;
    }

    //回避ボタンが押されたら回避へ遷移
    if (input_.IsTriggered("dodge"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateDodge>(pPlayer_, input_, camera_));
        return;
    }

    //移動入力が完全に無くなったらIdleへ遷移
    if (!input_.HasMoveInput())
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
        return;
    }
}

void PlayerStateRun::Exit()
{
}
