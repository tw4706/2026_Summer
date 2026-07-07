#include "PlayerStateGuard.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include<string_view>

namespace
{
	//ガードのアニメーション
	const std::wstring_view kPlayerGuard = L"Player|Guard";

	//左右のガードアニメーション
	const std::wstring_view kPlayerLeftGuard = L"Player|LeftGuard";
	const std::wstring_view kPlayerRightGuard = L"Player|RightGuard";

	//移動速度
	constexpr float kMoveSpeed = 5.0f;
}

PlayerStateGuard::PlayerStateGuard(std::weak_ptr<Player> pPlayer, PlayerCamera& camera) :
	PlayerStateBase(pPlayer, camera)
{
}

void PlayerStateGuard::Enter()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	player->ChangeAnimation(AnimationState::Guard, kPlayerGuard.data());
	player->isGuarding_ = true;
}

void PlayerStateGuard::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	Vector3 playerDir = GetCameraLookMoveDirection();

	//正面ベクトルを今向いている向きで定義
	Vector3 forwardVec = { std::sinf(player->moveAngle_),0.0f,-std::cosf(player->moveAngle_) };

	//移動の入力がない場合はIdleに遷移
	if (playerDir.LengthSq() <= 0.0001f)
	{
		auto nextState = std::make_shared<PlayerStateIdle>(pPlayer_, camera_);
		player->ChangeState(nextState);
		return;
	}

	playerDir.Normalize();

	//移動速度を計算
	Vector3 moveVec = playerDir * kMoveSpeed;
	//移動後の位置を計算
	Vector3 nextPos = player->GetPos() + moveVec;
	//位置の適用
	player->SetPos(nextPos);

	//正面のベクトルと移動の入力ベクトルの外積を計算
	Vector3 cross = forwardVec.Cross(playerDir);

	//Y成分で右移動か左移動かを判定してアニメーションを遷移させる
	if (cross.y_ > 0.0f)
	{
		player->ChangeAnimation(AnimationState::RightGuard, kPlayerRightGuard.data());
	}
	else
	{
		player->ChangeAnimation(AnimationState::LeftGuard, kPlayerLeftGuard.data());
	}

	// アニメーションが終了したら
	if (player->IsAnimationEnd())
	{
		//Idleに遷移させる
		auto nextState = std::make_shared<PlayerStateIdle>(pPlayer_, camera_);
		player->ChangeState(nextState);
	}
}

void PlayerStateGuard::Exit()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	player->isGuarding_ = false;
}
