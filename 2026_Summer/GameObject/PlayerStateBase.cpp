#include "PlayerStateBase.h"
#include "Player.h"

PlayerStateBase::PlayerStateBase(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera)
    : CharacterStateBase(pPlayer),
    pPlayer_(pPlayer),
    input_(input),
    camera_(camera)
{
}
