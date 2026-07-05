#include "PlayerStateDeath.h"
#include "Player.h"

namespace
{
	//死亡アニメーション
	const wchar_t* kPlayerDeath = L"Player|Death";

}

PlayerStateDeath::PlayerStateDeath(std::weak_ptr<Player> pPlayer, Input& input, PlayerCamera& camera):
	PlayerStateBase(pPlayer,input,camera)
{
}

void PlayerStateDeath::Enter()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//速度をゼロに
	player->SetVelocity({ 0.0f,0.0f,0.0f });
	player->ChangeAnimation(AnimationState::Death, kPlayerDeath);
}

void PlayerStateDeath::Update()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//アニメーションが終了したら死ぬ
	if (player->IsAnimationEnd())
	{
		player->Death();
	}
}

void PlayerStateDeath::Exit()
{
}
