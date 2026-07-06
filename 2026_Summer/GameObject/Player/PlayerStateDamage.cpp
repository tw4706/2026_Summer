#include "PlayerStateDamage.h"
#include "PlayerStateIdle.h"
#include"Player/Player.h"

namespace
{
	//Damageアニメーション
	const wchar_t* kPlayerDamage = L"Player|Hit";
}

PlayerStateDamage::PlayerStateDamage(std::weak_ptr<Player> pPlayer, PlayerCamera& camera):
	PlayerStateBase(pPlayer,camera)
{
}

void PlayerStateDamage::Enter()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//状態遷移
	player->ChangeAnimation(AnimationState::Damage, kPlayerDamage);

	player->isInvincible_ = true;
}

void PlayerStateDamage::Update()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	if (player->IsAnimationEnd())
	{
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, camera_));
	}
}

void PlayerStateDamage::Exit()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	player->isInvincible_=false;
}
