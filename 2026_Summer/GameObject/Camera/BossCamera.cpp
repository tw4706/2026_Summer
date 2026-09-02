#include "BossCamera.h"
#include "Player/Player.h"
#include "Enemy/EnemyBase.h"
#include "CameraManager.h"

namespace
{
	//ボス演出の時間
	constexpr float kBossEventTime = 2.0f;

	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//補間の割合
	constexpr float kPosLerpRate = 0.1f;
	constexpr float kTargetLerpRate = 0.1f;

	//ボスとプレイヤーの中間地点を注視点にする割合
	constexpr float kBossFocusRate = 0.5f;

	//注視点の高さオフセット
	constexpr float kHeightOffset = 150.0f;
}

BossCamera::BossCamera()
{
}

void BossCamera::Init()
{
	//時間の設定
	bossEventTimer_ = kBossEventTime;
	cameraTarget_ = pos_;
	lerpPos_ = pos_;
}

void BossCamera::Update(int stageModelHandle)
{
	auto player = pPlayer_.lock();
	auto boss = pBoss_.lock();
	if (!player || !boss) return;

	Vector3 playerPos = player->GetCameraTarget();
	Vector3 bossPos = boss->GetCameraTarget();

	//中間地点を注視点にする
	Vector3 targetCameraTarget = Vector3::Lerp(playerPos, bossPos, kBossFocusRate);

	//プレイヤーとボスの差分ベクトル(この真ん中の地点をボスカメラは見る)
	Vector3 diff = bossPos - playerPos;
	diff.y_ = 0.0f;
	float dist = diff.Length();
	Vector3 dir = (dist > 0.0001f) ? diff.Normalize() : Vector3{ 0.0f, 0.0f, 1.0f };

	//カメラのターゲット位置の計算
	Vector3 sideOffset = Vector3{ -dir.x_, 0.0f, -dir.z_ } *150.0f;
	Vector3 targetPos = targetCameraTarget + sideOffset;

	//現在値から目標値へ補間
	cameraTarget_ = Vector3::Lerp(cameraTarget_, targetCameraTarget, kTargetLerpRate);
	lerpPos_ = Vector3::Lerp(lerpPos_, targetPos, kPosLerpRate);

	//ステージとの衝突チェック
	pos_ = CheckCollCameraToStage(stageModelHandle, cameraTarget_, lerpPos_);

	//描画の反映
	UpdateRenderSystem();

	//演出時間が経過したらプレイヤーカメラへ戻す
	bossEventTimer_ -= kDeltaTime;
	if (bossEventTimer_ <= 0.0f && pCameraManager_)
	{
		pCameraManager_->ChangeCamera(L"PlayerCamera");
	}
}

void BossCamera::SetTarget(std::weak_ptr<Player> pPlayer, std::weak_ptr<EnemyBase> pBoss, CameraManager* pCameraManager)
{
	pPlayer_ = pPlayer;
	pBoss_ = pBoss;

	pCameraManager_ = pCameraManager;
}
