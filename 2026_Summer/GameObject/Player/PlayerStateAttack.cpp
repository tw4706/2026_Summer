#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"

PlayerStateAttack::PlayerStateAttack(std::weak_ptr<Player> pPlayer, Input& input, CameraBase& camera) :
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateAttack::Enter()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    // ‡@ UŒ‚ƒAƒjƒ[ƒVƒ‡ƒ“‚ÉØ‚è‘Ö‚¦
    pPlayer->ChangeAnimation(AnimationState::Attack);

    // ‡A UŒ‚ŠJŽnŽž‚ÉˆÚ“®‘¬“x‚ðŠ®‘S‚Éƒ[ƒ‚É‚·‚é (Œ³‚Ì Player::Move “à‚Ìˆ—‚ðˆÚA)
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

void PlayerStateAttack::Update()
{
    auto pPlayer = pPlayer_.lock();
    if (!pPlayer) return;

    //UŒ‚’†‚ÍˆÚ“®“ü—Í‚ðˆêØŽó‚¯•t‚¯‚¸A‘¬“x‚àƒ[ƒ‚ðˆÛŽ‚·‚é
    pPlayer->SetVelocity({ 0.0f, 0.0f, 0.0f });

    //ƒAƒjƒ[ƒVƒ‡ƒ“‚ªI—¹‚µ‚½‚çIdleó‘Ô‚Ö–ß‚é
    if (pPlayer->IsAnimationEnd())
    {
        pPlayer->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
        return;
    }
}

void PlayerStateAttack::Exit()
{
}
