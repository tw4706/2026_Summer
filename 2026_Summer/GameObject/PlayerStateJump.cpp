#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "Matrix4x4.h"

PlayerStateJump::PlayerStateJump(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera) :
    PlayerStateBase(pPlayer, input, camera)
{
}

void PlayerStateJump::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    // アニメーションをジャンプに切り替える
    // (Player側で一括管理している場合は、Playerに通知するメンバ関数を呼ぶ)
    pPlayer->ChangeAnimation(AnimationState::Jump);

    // ----------------------------------------------------
    // ① ジャンプの初速を与える処理 (Player::Jump から移植)
    // ----------------------------------------------------
    if (pPlayer->GetIsGround())
    {
        Vector3 vel = pPlayer->GetVelocity();
        vel.y_ = pPlayer->GetJumpPower(); // 初速をセット (ゲッターが必要)
        pPlayer->SetVelocity(vel);

        pPlayer->SetIsGround(false); // 空中フラグを立てる
    }
}

void PlayerStateJump::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    // ----------------------------------------------------
    // 【追加】空中での旋回・移動制御 (Runから移植・調整)
    // ----------------------------------------------------
    Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
    if (input_.IsPressed("up"))    inputDir.z_ += 1.0f;
    if (input_.IsPressed("down"))  inputDir.z_ -= 1.0f;
    if (input_.IsPressed("left"))  inputDir.x_ -= 1.0f;
    if (input_.IsPressed("right")) inputDir.x_ += 1.0f;

    bool isKeyboardMoving = (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > 0.01f);
    Vector3 currentVel = pPlayer->GetVelocity();
    float currentAngle = pPlayer->GetMoveAngle();

    if (isKeyboardMoving)
    {
        float cameraYaw = camera_.GetYaw();
        Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
        Vector3 playerDir = rotMat.TransformForVector(-inputDir).Normalize();

        // 空中なので、地上（10.0f）より少し遅い速度制限にするか、Lerpを鈍くする
        Vector3 targetVel = playerDir * 8.0f;
        currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, 0.05f); // 0.15fより小さくして慣性を出す
        currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, 0.05f);

        // 旋回（向きの変更）処理
        float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
        float diff = playerAngle - currentAngle;
        while (diff > DX_PI_F) diff -= DX_PI_F * 2;
        while (diff < -DX_PI_F) diff += DX_PI_F * 2;

        currentAngle += diff * 0.15f; // 空中なので少しゆっくり回るように調整
    }

    // ----------------------------------------------------
    // 従来の重力・位置更新処理
    // ----------------------------------------------------
    Vector3 pos = pPlayer->GetPosition();
    currentVel.y_ -= pPlayer->GetGravity(); // 重力をY軸に適用

    // 地面との接地判定
    if (pos.y_ + currentVel.y_ <= 0.0f)
    {
        pos.y_ = 0.0f;
        currentVel.y_ = 0.0f;
        pPlayer->SetIsGround(true);
    }

    // 計算結果を反映
    pPlayer->SetVelocity(currentVel);
    pPlayer->SetMoveAngle(currentAngle); // 変更した向きを適用
    pPlayer->AddPosition(currentVel);

    // カメラ回転（ジャンプ中もカメラを回せるように）
    Vector3 stickR = input_.GetStickRight();
    camera_.AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);

    // ----------------------------------------------------
    // ③ 状態遷移判定
    // ----------------------------------------------------
    if (pPlayer->GetIsGround())
    {
        // 着地した瞬間の速度によって、Idleに戻るか、そのままRunに移行するかを分岐させる
        float speedXZ = sqrtf(currentVel.x_ * currentVel.x_ + currentVel.z_ * currentVel.z_);
        if (speedXZ > 1.5f)
        {
            pPlayer->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_, input_, camera_));
        }
        else
        {
            pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
        }
        return;
    }
}

void PlayerStateJump::Exit()
{
}
