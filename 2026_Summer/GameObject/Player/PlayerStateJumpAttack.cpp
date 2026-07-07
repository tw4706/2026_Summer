#include "PlayerStateJumpAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"

namespace
{
	//JumpAttackアニメーション
	const std::wstring_view kPlayerJumpAttack = L"Player|JumpAttack";
}

PlayerStateJumpAttack::PlayerStateJumpAttack(std::weak_ptr<Player> pPlayer, PlayerCamera& camera):
	PlayerStateBase(pPlayer, camera)
{
}

void PlayerStateJumpAttack::Enter()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	player->ChangeAnimation(AnimationState::JumpAttack, kPlayerJumpAttack.data());

	//攻撃開始と同時に刀の当たり判定を有効化
	player->SetKatanaColliderEnabled(true);
}

void PlayerStateJumpAttack::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//攻撃開始時に移動速度を0にする
	player->SetVelocity({0.0f,0.0f,0.0f});

	//アニメーションが終了したらIdle状態へ戻る
	if (player->IsAnimationEnd())
	{
		//刀の当たり判定を無効化
		player->SetKatanaColliderEnabled(false);
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, camera_));
		return;
	}
	if (player->GetIsGround())
	{
		Vector3 vel = player->GetVelocity();
		vel.y_ = 0.0f;
		player->SetVelocity(vel);
	}

}

void PlayerStateJumpAttack::Exit()
{
	auto player = pPlayer_.lock();
	if (player)
	{
		player->SetKatanaColliderEnabled(false);
	}
}
