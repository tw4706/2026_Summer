#include "PlayerStateIdle.h"
#include "Player.h"
#include<Dxlib.h>
#include<array>
#include<memory>

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<Player>pPlayer):
	PlayerStateBase(pPlayer)
{
	auto player = pPlayer_.lock();
	if (!player)return;
}

PlayerStateIdle::~PlayerStateIdle()
{
}

void PlayerStateIdle::Init()
{
	auto player = pPlayer_.lock();
	if (!player)return;
}

void PlayerStateIdle::Update()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//デバッグ
	DrawFormatString(0, 100, 0xffffff, "Idle");

	std::array<char, 256>keyState;
	GetHitKeyStateAll(keyState.data());
	if (keyState[KEY_INPUT_F])
	{
		//ステートの遷移
		player->ChangePlayerState(std::make_shared<PlayerStateMove>(pPlayer_));
		printf("状態遷移した");
		return;
	}
}

void PlayerStateIdle::End()
{
}
