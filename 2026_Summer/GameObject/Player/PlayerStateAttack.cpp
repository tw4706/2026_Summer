#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "PlayerStateGuard.h"
#include "ComboManager.h"
#include "SoundManager.h"
#include "Player.h"
#include "Input.h"

PlayerStateAttack::PlayerStateAttack(std::weak_ptr<Player> pPlayer) :
	PlayerStateBase(pPlayer)
{
}

void PlayerStateAttack::Enter()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	//ロックオン中は常に敵のほうを向く
	player->LooktoLockOnEnemy();

	ComboManager& combo = player->GetComboManager();
	const ComboData* data = combo.GetCurrentComboData();

	if (!data)
	{
		//コンボのリセット
		combo.ResetCombo();
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_));
		return;
	}

	//攻撃アニメーションに遷移
	player->ChangeAnimation(AnimationState::Attack, data->animName.c_str());

	//速度を0にする
	player->SetVelocity(Vector3{ 0.0f, 0.0f, 0.0f });

	//攻撃行動を記録
	player->actionCounter_.RecordAttack();

	//当たり判定は指定したフレームから有効にするため最初は無効化
	player->SetKatanaColliderEnabled(false);

	//コンボのフラグをクリア
	combo.ClearNextComboRequest();

	isEffectPlaying_ = false;
	isColliderEnabled_ = false;
	isAttackSePlayed_ = false;
}

void PlayerStateAttack::Update()
{
	auto player = pPlayer_.lock();
	if (!player) return;

	ComboManager& combo = player->GetComboManager();
	const ComboData* data = combo.GetCurrentComboData();
	if (!data)
	{
		combo.ResetCombo();
		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_));
		return;
	}

	//現在のアニメーションフレームを取得
	float currentFrame = player->GetAnimationCurrentTime();

	//ガード入力が行われたら攻撃をキャンセルしてガード状態へ遷移
	if (Input::GetInstance().IsTriggered("guard"))
	{
		//攻撃演出を止めてからガードへキャンセル遷移
		player->SetKatanaColliderEnabled(false);
		player->StopKatanaEffect();

		//コンボのリセット
		combo.ResetCombo();

		player->ChangeState(std::make_shared<PlayerStateGuard>(pPlayer_));
		return;
	}

	//攻撃入力が行われたら
	if (Input::GetInstance().IsTriggered("attack"))
	{
		//コンボで入力を受け付ける
		combo.OnAttackInput(currentFrame);
	}

	//CSVで指定したフレームで攻撃SEを再生
	if (!isAttackSePlayed_ && currentFrame >= static_cast<float>(data->comboPlaySEFrame))
	{
		SoundManager::GetInstance().PlaySe(SE::Attack);
		isAttackSePlayed_ = true;
	}

	//CSVで指定したフレームでエフェクトの切り替えをする
	bool isEffectEnabled = (currentFrame >= static_cast<float>(data->effectStartFrame) &&
		currentFrame <= static_cast<float>(data->effectEndFrame));
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

	//コライダーが有効なフレームかつ現在有効じゃないとき
	bool isColliderEnableFrame = (currentFrame >= static_cast<float>(data->colliderStartFrame) &&
		currentFrame <= static_cast<float>(data->colliderEndFrame));

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


	player->AddPosition();

	player->SetVelocity({ 0.0f, currentVel.y_, 0.0f });

	if (player->GetIsGround())
	{
		Vector3 vel = player->GetVelocity();
		vel.y_ = 0.0f;
		player->SetVelocity(vel);
	}

	if (combo.CanTransToNextCombo(currentFrame) &&
		combo.IsNextComboRequested() &&!combo.IsMaxCombo())
	{
		//次段へ進む前に現在の演出を止めておく
		player->SetKatanaColliderEnabled(false);
		player->StopKatanaEffect();

		combo.AdvancedCombo();

		//Enterをもう一度通す形で次段の攻撃状態へ遷移
		player->ChangeState(std::make_shared<PlayerStateAttack>(pPlayer_));
		return;
	}

	//アニメーションが終了したらIdle状態へ戻る
	if (player->IsAnimationEnd())
	{
		//刀の当たり判定を無効化
		player->SetKatanaColliderEnabled(false);

		//コンボのリセット
		combo.ResetCombo();

		player->ChangeState(std::make_shared<PlayerStateIdle>(pPlayer_));
		return;
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
