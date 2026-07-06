#include "PlayerStateGuard.h"
#include "PlayerStateIdle.h"
#include "Player.h"

namespace
{
	//ガードのアニメーション
	const wchar_t* kPlayerGuard = L"Player|Guard";
}

PlayerStateGuard::PlayerStateGuard(std::weak_ptr<Player> pPlayer, PlayerCamera& camera):
	PlayerStateBase(pPlayer,camera)
{
}

void PlayerStateGuard::Enter()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	player->ChangeAnimation(AnimationState::Guard, kPlayerGuard);
	player->SetIsGuarding(true);
}

void PlayerStateGuard::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//ガードのアニメーションが終了したらIdle状態に戻る
	if (player->IsAnimationEnd())
	{
		auto nextState = std::make_shared<PlayerStateIdle>(pPlayer_,camera_);
		player->ChangeState(nextState);
	}

}

void PlayerStateGuard::Exit()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	player->SetIsGuarding(false);
}
