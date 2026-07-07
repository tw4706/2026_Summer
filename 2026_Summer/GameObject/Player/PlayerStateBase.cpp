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

	//ƒJƒƒ‰‚ÌYawŠp‚ðŽg‚Á‚½s—ñ‰ñ“]‚ðs‚¤
	float cameraYaw = pCamera_.GetYaw();					//ƒJƒƒ‰‚Ìƒˆ[Šp‚ðŽæ“¾
	Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);	//YŽ²‰ñ“]s—ñ‚ðì¬

	Vector3 moveDir = rotMat.TransformForVector(-rawInput).Normalize();

	return moveDir;
}
