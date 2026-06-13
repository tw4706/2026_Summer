#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"

namespace
{
    //入力のしきい値
    constexpr float kInputEpsilon = 0.01f;

    //アナログスティックのデッドゾーン
    constexpr float kStickDeadZone = 0.15f;

    //移動時の線形補間
    constexpr float kMoveLerp = 0.15f;

    //カメラの回転速度
    constexpr float kCameraSpeed = 0.03f;
}

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera) :
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateIdle::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    pPlayer->ChangeAnimation(AnimationState::Idle);

    //速度を0にする
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

void PlayerStateIdle::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    //カメラの回転
    Vector3 stickR = input_.GetStickRight();
    camera_.AddRotation(-stickR.x_ * kCameraSpeed, -stickR.z_ * kCameraSpeed);

    //空中にいるときかジャンプが押されたらJump状態へ遷移
    if (!pPlayer->GetIsGround() || input_.IsTriggered("jump"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    //攻撃が押されたらAttack状態へ遷移
    if (input_.IsTriggered("attack"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, input_, camera_));
        return;
    }

    //キーボードかアナログスティックの入力があったらRun状態へ遷移
    Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
    if (input_.IsPressed("up"))    inputDir.z_ += 1.0f;
    if (input_.IsPressed("down"))  inputDir.z_ -= 1.0f;
    if (input_.IsPressed("left"))  inputDir.x_ -= 1.0f;
    if (input_.IsPressed("right")) inputDir.x_ += 1.0f;
    bool isKeyboardMoving = (fabs(inputDir.x_) > kInputEpsilon || fabs(inputDir.z_) > kInputEpsilon);

    Vector3 stickL = input_.GetStickLeft();
    bool isStickMoving = (stickL.LengthSq() > kStickDeadZone);

    if (isKeyboardMoving || isStickMoving)
    {
        // 入力が入った瞬間、実際の移動計算・加速はすべて Run ステートに丸投げする
        pPlayer->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_, input_, camera_));
        return;
    }
}

void PlayerStateIdle::Exit()
{
}
