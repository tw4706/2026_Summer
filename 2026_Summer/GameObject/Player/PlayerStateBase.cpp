#include "PlayerStateBase.h"
#include "Player.h"
#include "Vector3.h"
#include "Input.h"
#include "Camera/CameraBase.h"
#include "Matrix4x4.h"
#include<Dxlib.h>

PlayerStateBase::PlayerStateBase(Player* pPlayer, Input& input, CameraBase& camera)
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

	if (!pPlayer_) return { 0.0f, 0.0f, 0.0f };

	//ƒJƒƒ‰‚ÌYawŠp‚ðŽg‚Á‚½s—ñ‰ñ“]‚ðs‚¤
	float cameraYaw = camera_.GetYaw();					//ƒJƒƒ‰‚Ìƒˆ[Šp‚ðŽæ“¾
	Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);	//YŽ²‰ñ“]s—ñ‚ðì¬

	Vector3 moveDir = rotMat.TransformForVector(-rawInput).Normalize();

	return moveDir;
}
