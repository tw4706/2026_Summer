#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "Player.h"

namespace
{
	//Attack
	const std::wstring_view kPlayerAttack = L"Player|Attack";

	//刀エフェクトを再生し始めるフレーム
	constexpr float kEffectStartFrame = 17.0f;

	//刀エフェクトを止めるフレーム
	constexpr float kEffectEndFrame = 25.0f;

	//当たり判定を有効にするフレーム
	constexpr float kColliderStartFrame = 17.0f;

	//当たり判定を無効にするフレーム
	constexpr float kColliderEndFrame = 25.0f;
}

PlayerStateAttack::PlayerStateAttack(std::weak_ptr<Player> pPlayer) :
	PlayerStateBase(pPlayer)
{
}

void PlayerStateAttack::Enter()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//攻撃アニメーションに遷移
	player->ChangeAnimation(AnimationState::Attack, kPlayerAttack.data());

	//速度を0にする
	player->SetVelocity(Vector3{ 0.0f, 0.0f, 0.0f });

	//攻撃行動を記録
	player->actionCounter_.RecordAttack();

	//当たり判定は指定したフレームから有効にするため最初は無効化
	player->SetKatanaColliderEnabled(false);
	isEffectPlaying_ = false;
	isColliderEnabled_ = false;
}

void PlayerStateAttack::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//現在のアニメーションフレームを取得
	float currentFrame = player->GetAnimationCurrentTime();

	//指定フレームでエフェクトの切り替え
	bool isEffectEnabled = (currentFrame >= kEffectStartFrame && currentFrame <= kEffectEndFrame);
	if (isEffectEnabled && !isEffectPlaying_)
	{
		player->PlayKatanaEffect();
		isEffectPlaying_ = true;
	}
	else if (!isEffectEnabled && isEffectPlaying_)
	{
		player->StopKatanaEffect();
		isEffectPlaying_ = false;
	}

	//コライダーが有効なフレームかつ現在有効でない場合
	bool isColliderEnableFrame = (currentFrame >= kColliderStartFrame && currentFrame <= kColliderEndFrame);

	// コライダーが出せるときは
	if (isColliderEnableFrame && !isColliderEnabled_)
	{
		player->SetKatanaColliderEnabled(true);
		isColliderEnabled_ = true;
	}
	else if (!isColliderEnableFrame && isColliderEnabled_)
	{
		player->SetKatanaColliderEnabled(false);
		isColliderEnabled_ = false;
	}

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
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_));
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

		pPlayer->StopKatanaEffect();
	}
}
