#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"

PlayerStateAttack::PlayerStateAttack(Player* pPlayer, Input& input, CameraBase& camera) :
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateAttack::Enter()
{
    if (!pPlayer_) return;

    // ‡@ UŒ‚ƒAƒjƒ[ƒVƒ‡ƒ“‚ÉØ‚è‘Ö‚¦
    pPlayer_->ChangeAnimation(AnimationState::Attack);

    // ‡A UŒ‚ŠJŽnŽž‚ÉˆÚ“®‘¬“x‚ðŠ®‘S‚Éƒ[ƒ‚É‚·‚é (Œ³‚Ì Player::Move “à‚Ìˆ—‚ðˆÚA)
    pPlayer_->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

void PlayerStateAttack::Update()
{
    if (!pPlayer_) return;

    //UŒ‚’†‚ÍˆÚ“®“ü—Í‚ðˆêØŽó‚¯•t‚¯‚¸A‘¬“x‚àƒ[ƒ‚ðˆÛŽ‚·‚é
    pPlayer_->SetVelocity({ 0.0f, 0.0f, 0.0f });

    //ƒAƒjƒ[ƒVƒ‡ƒ“‚ªI—¹‚µ‚½‚çIdleó‘Ô‚Ö–ß‚é
    if (pPlayer_->IsAnimationEnd())
    {
        pPlayer_->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
        return;
    }
}

void PlayerStateAttack::Exit()
{
}
