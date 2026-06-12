#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera) :
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateIdle::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    pPlayer->ChangeAnimation(AnimationState::Idle);

    // 完全に静止させる（念のため）
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

void PlayerStateIdle::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    // ----------------------------------------------------
    // ① 共通のカメラ回転だけは Idle 中も行う
    // ----------------------------------------------------
    Vector3 stickR = input_.GetStickRight();
    camera_.AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);

    // ----------------------------------------------------
    // ② 状態遷移判定（入力があったら対応するステートへ投げる）
    // ----------------------------------------------------

    // 1. 空中にいる、またはジャンプが押されたら Jump へ
    if (!pPlayer->GetIsGround() || input_.IsTriggered("jump"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    // 2. 攻撃が押されたら Attack へ
    if (input_.IsTriggered("attack"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, input_, camera_));
        return;
    }

    // 3. キーボードかアナログスティックの「移動入力」があったら Run へ！
    Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
    if (input_.IsPressed("up"))    inputDir.z_ += 1.0f;
    if (input_.IsPressed("down"))  inputDir.z_ -= 1.0f;
    if (input_.IsPressed("left"))  inputDir.x_ -= 1.0f;
    if (input_.IsPressed("right")) inputDir.x_ += 1.0f;
    bool isKeyboardMoving = (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > 0.01f);

    Vector3 stickL = input_.GetStickLeft();
    bool isStickMoving = (stickL.LengthSq() > 0.15f); // kStickDeadZone

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
