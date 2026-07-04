#include "PlayerStateDamage.h"
#include "PlayerStateIdle.h"
#include"Player/Player.h"

namespace
{
	//Damageアニメーション
	const wchar_t* kPlayerDamage = L"Player|Hit";
}
PlayerStateDamage::PlayerStateDamage(std::weak_ptr<Player> pPlayer, Input& input, PlayerCamera& camera):
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateDamage::Enter()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//状態遷移
	player->ChangeAnimation(AnimationState::Damage, kPlayerDamage);

	player->SetIsInvincible(true);
}

void PlayerStateDamage::Update()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	if (player->IsAnimationEnd())
	{
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
	}
}

void PlayerStateDamage::Exit()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	player->SetIsInvincible(false);
}
