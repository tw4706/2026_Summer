#include "PlayerStateBase.h"
#include"Player.h"

PlayerStateBase::PlayerStateBase(std::weak_ptr<Player>pPlayer):
	pPlayer_(pPlayer)
{
}

PlayerStateBase::~PlayerStateBase()
{
}
