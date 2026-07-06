#include "PlayerStateDodge.h"
#include "Player.h"
#include "Input.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"

namespace
{
	//回避アニメーション
	const wchar_t* kPlayeDodge = L"Player|Dodge";

	//回避速度
	constexpr float kDodgeSpeed = 15.0f;

	//無敵時間
	constexpr float kDodgeDurataion = 0.3f;

	//硬直時間
	constexpr float kStiffnessDurataion = 0.0f;

	//フレームカウントの進める時間
	constexpr float kFrameTime = 1.0f / 60.0f;
}

PlayerStateDodge::PlayerStateDodge(std::weak_ptr<Player> pPlayer, PlayerCamera& camera) :
	PlayerStateBase(pPlayer, camera),
	invincibleTimer_(0.0f)
{
}

void PlayerStateDodge::Enter()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	player->ChangeAnimation(AnimationState::Dodge, kPlayeDodge);

	//無敵時間のセット
	player->isInvincible_ = true;

	//現在カメラ基準の入力方向を直接取得する
	Vector3 moveDir = GetCameraLookMoveDirection();

	//入力がない、または直立状態ならプレイヤーの向いている正面方向にする
	if (moveDir.LengthSq() < 0.001f)
	{
		float angle = player->moveAngle_;
		moveDir = Vector3(sinf(angle), 0.0f, -cosf(angle));
	}

	//正規化
	moveDir = moveDir.Normalize();

	//回避速度をセット
	player->SetVelocity(Vector3(moveDir.x_ * kDodgeSpeed, 0.0f, moveDir.z_ * kDodgeSpeed));

	//回避をするときにモデルも回避方向に向くように調整する
	float targetAngle = atan2f(moveDir.x_, -moveDir.z_);
	player->moveAngle_ = targetAngle;

	//タイマーをリセット
	invincibleTimer_ = 0.0f;
}

void PlayerStateDodge::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//タイマーを進める
	invincibleTimer_ += kFrameTime;

	//無敵時間のタイマーが回避時間より短い場合
	if (invincibleTimer_ < kDodgeDurataion)
	{

	}
	//回避時間が終了したら
	else
	{
		//無敵フラグを解除
		player->isInvincible_ = false;

		//入力の有無に応じて状態遷移を行う
		if (Input::GetInstance().HasMoveInput())
		{
			auto nextState = std::make_shared<PlayerStateRun>(pPlayer_, camera_);
			player->ChangeState(nextState);
			nextState->Update();
		}
		else
		{
			//止まっている場合のみ速度を0にする
			player->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			auto nextState = std::make_shared<PlayerStateIdle>(pPlayer_, camera_);
			player->ChangeState(nextState);
			nextState->Update();
		}
		return;
	}

	//座標の更新
	player->AddPosition();
}

void PlayerStateDodge::Exit()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//ここでも一応のために無敵を解除しておく
	player->isInvincible_ = false;
}
