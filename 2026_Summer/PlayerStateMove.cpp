#include "PlayerStateMove.h"
#include "Player.h"
#include<Dxlib.h>
#include<array>
#include<memory>

PlayerStateMove::PlayerStateMove(std::weak_ptr<Player> pPlayer):
	PlayerStateBase(pPlayer)
{
}

PlayerStateMove::~PlayerStateMove()
{
}


void PlayerStateMove::Init()
{
}

void PlayerStateMove::Update()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//デバッグ
	DrawFormatString(0, 100, 0xffffff, "Move");

	std::array<char, 256>keyState;
	GetHitKeyStateAll(keyState.data());
	if (keyState[KEY_INPUT_F])
	{
		//ステートの遷移
		player->ChangePlayerState(std::make_shared<PlayerStateIdle>(pPlayer_));
		printf("状態遷移した");
		return;
	}
}

void PlayerStateMove::End()
{
}