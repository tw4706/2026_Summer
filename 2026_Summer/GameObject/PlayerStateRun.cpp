#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "Matrix4x4.h"

namespace
{
    //プレイヤーの移動速度
    constexpr float kSpeed = 10.0f;

    //停止する際の線形補間
    constexpr float kStopLerp = 0.2f;

    //カメラの回転速度
    constexpr float kCameraSpeed = 0.03f;
    constexpr float kCameraPitch = 0.3f;

    constexpr float kRotateLerpAnalogStick = 0.3f;

    //移動の入力のしきい値
    constexpr float kRunEpsilon = 0.01f;

    //移動時の線形補間
    constexpr float kMoveLerp = 0.15f;

    //アナログスティックのデッドゾーン
    constexpr float kStickDeadZone = 0.15f;
}

PlayerStateRun::PlayerStateRun(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera) :
    PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateRun::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    pPlayer->ChangeAnimation(AnimationState::Run);
}

void PlayerStateRun::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    //移動処理
    Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
    if (input_.IsPressed("up"))    inputDir.z_ += 1.0f;
    if (input_.IsPressed("down"))  inputDir.z_ -= 1.0f;
    if (input_.IsPressed("left"))  inputDir.x_ -= 1.0f;
    if (input_.IsPressed("right")) inputDir.x_ += 1.0f;

    bool isKeyboardMoving = (fabs(inputDir.x_) > kRunEpsilon || fabs(inputDir.z_) > kRunEpsilon);
    Vector3 currentVel = pPlayer->GetVelocity();
    float currentAngle = pPlayer->GetMoveAngle();

    //キーボードでの入力があった場合
    if (isKeyboardMoving)
    {
        float cameraYaw = camera_.GetYaw();//カメラのヨー角の取得
        Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);//Y軸回転
        Vector3 playerDir = rotMat.TransformForVector(-inputDir).Normalize();

        Vector3 targetVel = playerDir * kSpeed;
        currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, kMoveLerp);
        currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, kMoveLerp);

        float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
        float diff = playerAngle - currentAngle;
        while (diff > DX_PI_F) diff -= DX_PI_F * 2;
        while (diff < -DX_PI_F) diff += DX_PI_F * 2;

        currentAngle += diff * kRotateLerpAnalogStick;
    }
    else//パッドでの処理の場合
    {
        // アナログスティックの処理
        Vector3 stickL = input_.GetStickLeft();
        if (stickL.LengthSq() > kStickDeadZone)
        {
            float cameraYaw = camera_.GetYaw();
            Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
            Vector3 playerDir = rotMat.TransformForVector(-stickL).Normalize();

            float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
            float diff = playerAngle - currentAngle;
            while (diff > DX_PI_F) diff -= DX_PI_F * 2.0f;
            while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

            currentAngle += diff * kRotateLerpAnalogStick;

            Vector3 targetVel = playerDir * kSpeed;
            currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, kMoveLerp);
            currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, kMoveLerp);
        }
        else
        {
            //入力がない場合は減速させる
            currentVel.x_ = Vector3::Lerp(currentVel.x_, 0.0f, kStopLerp);
            currentVel.z_ = Vector3::Lerp(currentVel.z_, 0.0f, kStopLerp);
            if (currentVel.LengthSq() < kRunEpsilon) {
                currentVel = { 0.0f, 0.0f, 0.0f };
            }
        }
    }

    // 計算した速度と角度をPlayerに送り返し、位置を更新する
    pPlayer->SetVelocity(currentVel);
    pPlayer->SetMoveAngle(currentAngle);
    pPlayer->AddPosition(currentVel); // pos_ += vel_; の処理

    // カメラの回転処理もここで行う
    Vector3 stickR = input_.GetStickRight();
    camera_.AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);

    //ジャンプボタンの入力があった場合は
    if (input_.IsTriggered("jump"))
    {
        //ジャンプ状態に遷移
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    // プレイヤーが地面にいないときはジャンプ状態
    if (!pPlayer->GetIsGround())
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    //攻撃ボタンの入力があった場合は
    if (input_.IsTriggered("attack"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, input_, camera_));
        return;
    }

    //入力がない場合は
    bool isPadMoving = input_.GetStickLeft().LengthSq() > kStickDeadZone;
    if (!isKeyboardMoving && !isPadMoving)
    {
        //速度も0にしておく
        pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });
        //Idle状態に遷移する
        pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
        return;
    }

}

void PlayerStateRun::Exit()
{
}
