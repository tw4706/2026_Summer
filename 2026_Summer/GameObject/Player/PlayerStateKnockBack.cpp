#include "PlayerStateKnockBack.h"
#include "PlayerStateIdle.h"
#include "Player.h"

namespace
{
	//ノックバックアニメーション
	const std::wstring_view kPlayerKnockBack = L"Player|KnockBack";
}

PlayerStateKnockBack::PlayerStateKnockBack(std::weak_ptr<Player> pPlayer) :
	PlayerStateBase(pPlayer)
{

}

void PlayerStateKnockBack::Enter()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	player->ChangeAnimation(AnimationState::KnockBack, kPlayerKnockBack.data());
}

void PlayerStateKnockBack::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//アニメーションが終了したら
	if (player->IsAnimationEnd())
	{
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_));
	}
}

void PlayerStateKnockBack::Exit()
{
}
