#include "PlayerStateRun.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "Matrix4x4.h"

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

    bool isKeyboardMoving = (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > 0.001f);
    Vector3 currentVel = pPlayer->GetVelocity();
    float currentAngle = pPlayer->GetMoveAngle();

    if (isKeyboardMoving)
    {
        float cameraYaw = camera_.GetYaw(); // メンバ変数のカメラから取得
        Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
        Vector3 playerDir = rotMat.TransformForVector(-inputDir).Normalize();

        Vector3 targetVel = playerDir * 10.0f; // kSpeedの代わり（定数クラス等から持ってくると良いです）
        currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, 0.15f); // kMoveLerp
        currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, 0.15f);

        float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
        float diff = playerAngle - currentAngle;
        while (diff > DX_PI_F) diff -= DX_PI_F * 2;
        while (diff < -DX_PI_F) diff += DX_PI_F * 2;

        currentAngle += diff * 0.3f; // kRotateLerpAnalogStick
    }
    else
    {
        // アナログスティックの処理
        Vector3 stickL = input_.GetStickLeft();
        if (stickL.LengthSq() > 0.15f) // kStickDeadZone
        {
            float cameraYaw = camera_.GetYaw();
            Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
            Vector3 playerDir = rotMat.TransformForVector(-stickL).Normalize();

            float playerAngle = atan2f(playerDir.x_, -playerDir.z_);
            float diff = playerAngle - currentAngle;
            while (diff > DX_PI_F) diff -= DX_PI_F * 2.0f;
            while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

            currentAngle += diff * 0.3f;

            Vector3 targetVel = playerDir * 10.0f;
            currentVel.x_ = Vector3::Lerp(currentVel.x_, targetVel.x_, 0.15f);
            currentVel.z_ = Vector3::Lerp(currentVel.z_, targetVel.z_, 0.15f);
        }
        else
        {
            // 入力がない場合は減速
            currentVel.x_ = Vector3::Lerp(currentVel.x_, 0.0f, 0.2f); // kStopLerp
            currentVel.z_ = Vector3::Lerp(currentVel.z_, 0.0f, 0.2f);
            if (currentVel.LengthSq() < 0.01f) {
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
    camera_.AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);


    if (input_.IsTriggered("jump"))
    {
        // ※もし Player 側で Y軸の初速を与える仕様なら、ここで事前に SetIsGround(false) などを呼ぶか、
        // もしくは現状通り PlayerStateJump::Enter() 側の処理に任せます。
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    // （念のため、落下などで空中になった場合の安全弁として残す）
    if (!pPlayer->GetIsGround())
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateJump>(pPlayer_, input_, camera_));
        return;
    }

    // Attackへ
    if (input_.IsTriggered("attack"))
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_, input_, camera_));
        return;
    }

    float speedVec = sqrtf(currentVel.x_ * currentVel.x_ + currentVel.z_ * currentVel.z_);
    if (speedVec <= 0.0001f) 
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
        return;
    }

}

void PlayerStateRun::Exit()
{
}
