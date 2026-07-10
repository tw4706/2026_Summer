#include "PlayerStateDeath.h"
#include "Player.h"

namespace
{
	//死亡アニメーション
	const std::wstring_view kPlayerDeath = L"Player|Death";

}

PlayerStateDeath::PlayerStateDeath(std::weak_ptr<Player> pPlayer):
	PlayerStateBase(pPlayer)
{
}

void PlayerStateDeath::Enter()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//速度をゼロに
	player->SetVelocity({ 0.0f,0.0f,0.0f });
	player->ChangeAnimation(AnimationState::Death, kPlayerDeath.data());
}

void PlayerStateDeath::Update()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//アニメーションが終了したら
	if (player->IsAnimationEnd())
	{
		//プレイヤーを消す
		player->Destory();
	}

}

void PlayerStateDeath::Exit()
{
}
