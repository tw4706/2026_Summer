#include "PlayerStateBase.h"
#include "Player.h"
#include "Math/Vector3.h"
#include "Input.h"
#include "Camera/CameraBase.h"
#include "Math/Matrix4x4.h"
#include<Dxlib.h>

PlayerStateBase::PlayerStateBase(std::weak_ptr<Player> pPlayer, CameraBase& camera):
	CharacterStateBase(pPlayer),
    pPlayer_(pPlayer),
	pCamera_(camera)
{
}

Vector3 PlayerStateBase::GetCameraLookMoveDirection() const
{
	Vector3 rawInput = Input::GetInstance().GetRawMoveInput();
	if (rawInput.LengthSq() < 0.001f) return { 0.0f, 0.0f, 0.0f };

	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return { 0.0f, 0.0f, 0.0f };

	//今ロックオンしているかどうかのフラグ
	bool isLockOn = pPlayer->IsLockOn();

	if (isLockOn)
	{
		//カメラのターゲット座標を取得
		Vector3 cameraTarget = pCamera_.GetCameraTarget();

		Vector3 toEnemy = cameraTarget - pPlayer->GetPos();
		toEnemy.y_ = 0.0f;		//平面で移動させるためYは0
		toEnemy = toEnemy.Normalize();

		//敵の正面ベクトルから右のベクトルを計算
		Vector3 rightVec = Vector3{ 0.0f, 1.0f, 0.0f }.Cross(toEnemy).Normalize();

		//入力方向に応じて移動のベクトルを合成
		Vector3 moveDir = (toEnemy * rawInput.x_) + (rightVec * rawInput.z_);
		return moveDir.Normalize();
	}
	else
	{
		//カメラのYaw角を使った行列回転を行う
		float cameraYaw = pCamera_.GetYaw();				//カメラのヨー角を取得
		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);	//Y軸回転行列を作成

		Vector3 moveDir = rotMat.TransformForVector(-rawInput).Normalize();

		return moveDir;
	}

}
