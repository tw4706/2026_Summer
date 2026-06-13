#include "PlayerStateDodge.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"

namespace
{
	//回避速度
	constexpr float kDodgeSpeed = 1.5f;

	//無敵時間
	constexpr float kDodgeDurataion = 0.3f;

	//硬直時間
	constexpr float kStiffnessDurataion = 0.08f;

	//フレームカウントの進める時間
	constexpr float kFrameTime = 1.0f / 60.0f;
}

PlayerStateDodge::PlayerStateDodge(std::weak_ptr<Player> pPlayer, Input& input, Camera& camera):
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

	Vector3 moveDir = player->GetVelocity();
	if (moveDir.Length() < 0.001f)
	{
		//現在のプレイヤーが向いている角度を取得し向いている前方向に
		//回避を行う
		float angle = player->GetMoveAngle();
		moveDir = Vector3(sinf(angle), 0.0f, cosf(angle));
	}
	//正規化
	moveDir.Normalize();

	//回避速度を設定する
	player->SetVelocity(moveDir * kDodgeSpeed);

	//タイマーをリセット
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
