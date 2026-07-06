#include "PlayerStateGuard.h"

PlayerStateGuard::PlayerStateGuard(std::weak_ptr<Player> pPlayer, PlayerCamera& camera):
	PlayerStateBase(pPlayer,camera)
{
}

void PlayerStateGuard::Enter()
{
}

void PlayerStateGuard::Update()
{
}

void PlayerStateGuard::Exit()
{
}
