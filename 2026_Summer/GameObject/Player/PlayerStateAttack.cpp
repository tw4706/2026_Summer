#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"

namespace
{
	//Attack
	const std::wstring_view kPlayerAttack = L"Player|Attack";
}

PlayerStateAttack::PlayerStateAttack(std::weak_ptr<Player> pPlayer, CameraBase& camera) :
	PlayerStateBase(pPlayer, camera)
{
}

void PlayerStateAttack::Enter()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//攻撃アニメーションに遷移
	player->ChangeAnimation(AnimationState::Attack, kPlayerAttack.data());

	//攻撃開始と同時に刀の当たり判定を有効化
	player->SetKatanaColliderEnabled(true);
}

void PlayerStateAttack::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//攻撃開始時に移動速度を0にする
	Vector3 currentVel = player->GetVelocity();
	currentVel.y_ -= player->GetGravity();

	player->SetVelocity({ 0.0f, currentVel.y_, 0.0f });

	player->AddPosition();

	//アニメーションが終了したらIdle状態へ戻る
	if (player->IsAnimationEnd())
	{
		//刀の当たり判定を無効化
		player->SetKatanaColliderEnabled(false);
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, pCamera_));
		return;
	}
	if (player->GetIsGround())
	{
		Vector3 vel = player->GetVelocity();
		vel.y_ = 0.0f;
		player->SetVelocity(vel);
	}
}

void PlayerStateAttack::Exit()
{
	//念のためExitでも無効化
	auto pPlayer = pPlayer_.lock();
	if (pPlayer)
	{
		pPlayer->SetKatanaColliderEnabled(false);
	}
}
