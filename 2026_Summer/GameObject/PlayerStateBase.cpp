#include "PlayerStateBase.h"
#include "Player.h"
#include "Vector3.h"
#include "Input.h"
#include "Camera.h"
#include "Matrix4x4.h"
#include<Dxlib.h>

PlayerStateBase::PlayerStateBase(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera)
    : CharacterStateBase(pPlayer),
    pPlayer_(pPlayer),
    input_(input),
    camera_(camera)
{
}

Vector3 PlayerStateBase::GetCameraLookMoveDirection() const
{
	Vector3 rawInput = input_.GetRawMoveInput();
	if (rawInput.LengthSq() < 0.001f) return { 0.0f, 0.0f, 0.0f };

	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return { 0.0f, 0.0f, 0.0f };

	// 2. もともと Run ステートで成功していた「カメラのYaw角」を使った行列回転を行う
	float cameraYaw = camera_.GetYaw();              // カメラのヨー角を取得
	Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw); // Y軸回転行列を作成

	// もともとのRunクラスでは「-inputDir」を渡していたので、ここでも負のベクトルを渡します
	Vector3 moveDir = rotMat.TransformForVector(-rawInput).Normalize();

	return moveDir;
}
