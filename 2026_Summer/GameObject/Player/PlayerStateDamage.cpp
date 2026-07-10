#include "PlayerStateDamage.h"
#include "PlayerStateIdle.h"
#include"Player/Player.h"

namespace
{
	//Damageアニメーション
	const std::wstring_view kPlayerDamage = L"Player|Hit";
}

PlayerStateDamage::PlayerStateDamage(std::weak_ptr<Player> pPlayer):
	PlayerStateBase(pPlayer)
{
}

void PlayerStateDamage::Enter()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//状態遷移
	player->ChangeAnimation(AnimationState::Damage, kPlayerDamage.data());

	player->isInvincible_ = true;
}

void PlayerStateDamage::Update()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	if (player->IsAnimationEnd())
	{
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_));
	}
}

void PlayerStateDamage::Exit()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	player->isInvincible_=false;
}
