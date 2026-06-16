#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "Player.h"
#include "Input.h"
#include "Camera/CameraBase.h"
#include "Matrix4x4.h"

PlayerStateJump::PlayerStateJump(Player* pPlayer, Input& input, CameraBase& camera) :
    PlayerStateBase(pPlayer, input, camera)
{
}

void PlayerStateJump::Enter()
{
    if (!pPlayer_) return;

    // アニメーションをジャンプに切り替える
    pPlayer_->ChangeAnimation(AnimationState::Jump);

    if (pPlayer_->GetIsGround())
    {
        Vector3 vel = pPlayer_->GetVelocity();
        vel.y_ = pPlayer_->GetJumpPower(); //初速をセット
        pPlayer_->SetVelocity(vel);

        pPlayer_->SetIsGround(false); //接地フラグを返す
    }
}

void PlayerStateJump::Update()
{
    if (!pPlayer_) return;

    //空中での旋回・移動制御
    Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
    if (input_.IsPressed("up"))    inputDir.z_ += 1.0f;
    if (input_.IsPressed("down"))  inputDir.z_ -= 1.0f;
    if (input_.IsPressed("left"))  inputDir.x_ -= 1.0f;
    if (input_.IsPressed("right")) inputDir.x_ += 1.0f;

    bool isKeyboardMoving = (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > 0.01f);
    Vector3 currentVel = pPlayer_->GetVelocity();
    float currentAngle = pPlayer_->GetMoveAngle();

    if (isKeyboardMoving)
    {
        float cameraYaw = camera_.GetYaw();
        Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
        Vector3 playerDir = rotMat.TransformForVector(-inputDir).Normalize();

        Vector3 targetVel = playerDir * 8.0f;
        currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, 0.25f);
        currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, 0.25f);

        //向きの変更
        float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
        float diff = playerAngle - currentAngle;
        while (diff > DX_PI_F) diff -= DX_PI_F * 2;
        while (diff < -DX_PI_F) diff += DX_PI_F * 2;

        currentAngle += diff * 0.15f;
    }

    //重力・位置更新処理
    Vector3 pos = pPlayer_->GetPosition();
    currentVel.y_ -= pPlayer_->GetGravity(); //重力をY軸に適用

    //地面との接地判定
    if (pos.y_ + currentVel.y_ <= 0.0f)
    {
        pos.y_ = 0.0f;
        currentVel.y_ = 0.0f;
        pPlayer_->SetIsGround(true);
    }

    //計算結果を反映
    pPlayer_->SetVelocity(currentVel);
    pPlayer_->SetMoveAngle(currentAngle); //変更した向きを適用
    pPlayer_->AddPosition(currentVel);

    //カメラ回転
    Vector3 stickR = input_.GetStickRight();
    camera_.AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);

    //状態遷移判定
    if (pPlayer_->GetIsGround())
    {
        //着地した瞬間の速度によって、Idleに戻るか、そのままRunに移行するかを分岐させる
        float speedXZ = sqrtf(currentVel.x_ * currentVel.x_ + currentVel.z_ * currentVel.z_);
        if (speedXZ > 1.5f)
        {
            pPlayer_->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_, input_, camera_));
        }
        else
        {
            pPlayer_->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
        }
        return;
    }
}

void PlayerStateJump::Exit()
{
}
