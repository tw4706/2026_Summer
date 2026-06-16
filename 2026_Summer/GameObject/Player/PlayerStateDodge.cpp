#include "PlayerStateDodge.h"
#include "Player.h"
#include "Input.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"

namespace
{
	//回避速度
	constexpr float kDodgeSpeed = 15.0f;

	//無敵時間
	constexpr float kDodgeDurataion = 0.3f;

	//硬直時間
	constexpr float kStiffnessDurataion = 0.0f;

	//フレームカウントの進める時間
	constexpr float kFrameTime = 1.0f / 60.0f;
}

PlayerStateDodge::PlayerStateDodge(std::weak_ptr<Player> pPlayer, Input& input, CameraBase& camera):
	PlayerStateBase(pPlayer,input,camera),
	invincibleTimer_(0.0f)
{
}

void PlayerStateDodge::Enter()
{
	auto player = pPlayer_.lock();
	if (!player)return;

	//無敵時間のセット
	player->SetIsInvincible(true);

	//現在カメラ基準の入力方向を直接取得する
	Vector3 moveDir = GetCameraLookMoveDirection();

	//入力がない、または直立状態ならプレイヤーの向いている正面方向にする
	if (moveDir.LengthSq() < 0.001f)
	{
		float angle = player->GetMoveAngle();
		moveDir = Vector3(sinf(angle), 0.0f, cosf(angle));
	}

	//正規化
	moveDir = moveDir.Normalize();

	//回避速度をセット
	player->SetVelocity(Vector3(moveDir.x_ * kDodgeSpeed, 0.0f, moveDir.z_ * kDodgeSpeed));

	//回避を始めた瞬プレイヤーのモデルの向きも回避方向に一瞬で同期
	float targetAngle = atan2f(moveDir.x_, -moveDir.z_);
	player->SetMoveAngle(targetAngle);

	// タイマーをリセット
	invincibleTimer_ = 0.0f;
}

void PlayerStateDodge::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//タイマーを進める
	invincibleTimer_ += kFrameTime;

	if (invincibleTimer_ < kDodgeDurataion)
	{

	}
	else if(invincibleTimer_<(kDodgeDurataion+ kStiffnessDurataion))
	{
		//無敵中ならフラグを返して速度を0にする(今急停止すると思うので後々だんだんと止まるよう修正)
		if (player->GetIsInvincible())
		{
			player->SetIsInvincible(false);
			player->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
		}
	}
	else
	{
		//モデルが回避終了後ガクガクするため今モデルが向いている向きを再度設定する
		float currentAngle = player->GetMoveAngle();
		player->SetMoveAngle(currentAngle);

		if (input_.HasMoveInput())
		{
			player->ChangeState(std::make_shared<PlayerStateRun>(pPlayer_, input_, camera_));
		}
		else
		{
			player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_, input_, camera_));
		}
		return;
	}

	// 座標の更新
	player->AddPosition(player->GetVelocity());
}

void PlayerStateDodge::Exit()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//ここでも一応のために無敵を解除しておく
	player->SetIsInvincible(false);
}
