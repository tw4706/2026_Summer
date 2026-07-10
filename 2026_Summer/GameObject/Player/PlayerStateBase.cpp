#include "PlayerStateBase.h"
#include "Player.h"
#include "Enemy/EnemyBase.h"
#include "Math/Vector3.h"
#include "Input.h"
#include "Camera/CameraBase.h"
#include "Math/Matrix4x4.h"
#include<Dxlib.h>

PlayerStateBase::PlayerStateBase(std::weak_ptr<Player> pPlayer):
	CharacterStateBase(pPlayer),
    pPlayer_(pPlayer)
{
}

Vector3 PlayerStateBase::GetCameraLookMoveDirection() const
{
	Vector3 rawInput = Input::GetInstance().GetRawMoveInput();
	if (rawInput.LengthSq() < 0.001f) return { 0.0f, 0.0f, 0.0f };

	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return { 0.0f, 0.0f, 0.0f };

	//アクティブであるカメラの取得
	CameraBase* pCamera = GetActiveCamera();
	if (!pCamera) return { 0.0f, 0.0f, 0.0f };


	//今ロックオンしているかどうかのフラグ
	bool isLockOn = pPlayer->IsLockOn();

	if (isLockOn)
	{
		//カメラの向きで移動方向を計算
		Vector3 cameraPos = pCamera->GetPos();
		Vector3 cameraTargetPos = pCamera->GetCameraTarget();

		Vector3 forward = cameraTargetPos - cameraPos;
		forward.y_ = 0.0f;		//平面で移動させるためYは0

		//カメラの位置とターゲットがほぼ同じ場合はゼロ除算を避ける
		if (forward.LengthSq() < 0.0001f) return { 0.0f, 0.0f, 0.0f };
		forward = forward.Normalize();

		//敵の正面ベクトルから右のベクトルを計算
		Vector3 rightVec = Vector3{ 0.0f, 1.0f, 0.0f }.Cross(forward).Normalize();

		//入力方向に応じて移動のベクトルを合成
		Vector3 moveDir = (forward * rawInput.z_) + (rightVec * rawInput.x_);
		return moveDir.Normalize();
	}
	else
	{
		//カメラのYaw角を使った行列回転を行う
		float cameraYaw = pCamera->GetYaw();				//カメラのヨー角を取得
		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);	//Y軸回転行列を作成

		Vector3 moveDir = rotMat.TransformForVector(-rawInput).Normalize();

		return moveDir;
	}

}

CameraBase*PlayerStateBase::GetActiveCamera()const
{
	auto pPlayer = pPlayer_.lock();
	return pPlayer ? pPlayer->GetActiveCamera() : nullptr;
}
