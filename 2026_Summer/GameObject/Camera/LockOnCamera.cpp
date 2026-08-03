#include "LockOnCamera.h"
#include "Enemy/EnemyBase.h"
#include "Player/Player.h"
#include "Game.h"

namespace
{
	//lerpに割合(カメラのターゲット用)
	constexpr float kCameraTargetLerpRate = 0.1f;

	//正規化した視線方向のベクトル
	const float kMinDirLengthSq = 1.0f;

	//ターゲットまでの距離
	constexpr float kTargetDistance = 300.0f;

	//ターゲット座標のオフセット
	const Vector3 kTargetPosOffest = { 0.0f,100.0f,50.0f };

	//Lerpの割合(ターゲット座標の追従用)
	constexpr float kTargetPosLerpRate = 0.3f;

	//各デバッグ描画の定数
	constexpr int kDebugCameraPosY = 100;
	constexpr int kDebugTargetPosY = 130;
	constexpr int kDebugdirectionVecPosY = 160;
}

LockOnCamera::LockOnCamera()
{
}

LockOnCamera::~LockOnCamera()
{
}

void LockOnCamera::Init()
{
	lastDirectionVec_ = { 0.0f,0.0f,0.0f };
}

void LockOnCamera::Update(int stageModelHandle)
{
	auto player = pPlayer_.lock();
	auto enemy = pEnemy_.lock();

	//プレイヤーまたは敵が存在しない場合は何もしない
	if (!player || !enemy)return;

	//敵の座標を取得
	Vector3 enemyTargetPos = enemy->GetCameraTarget();

	//カメラのターゲット座標に代入
	cameraTarget_ = Vector3::Lerp(cameraTarget_,enemyTargetPos, kCameraTargetLerpRate);

	Vector3 playerCameraPos = player->GetCameraTarget();

	//プレイヤーから敵へ向かう方向ベクトルを作成
	Vector3 directionVec = playerCameraPos - cameraTarget_;

	directionVec.y_ = 0.0f;

	if (directionVec.LengthSq() < kMinDirLengthSq)
	{
		//前回の向きを保存
		directionVec = lastDirectionVec_;
	}
	else
	{
		directionVec = directionVec.Normalize();
		lastDirectionVec_ = directionVec; //保存
	}

	//ロックオン時のカメラのターゲット座標の計算
	Vector3 targetPos = playerCameraPos + (directionVec * kTargetDistance) + kTargetPosOffest;

	//線形補間を行い滑らかな追従を行う
	pos_ = Vector3::Lerp(pos_, targetPos, kTargetPosLerpRate);

	//カメラとステージの消灯判定を行う
	pos_ = CheckCollCameraToStage(stageModelHandle, playerCameraPos, pos_);

	//デバッグ描画
#ifdef _DEBUG
	DrawFormatString(0, kDebugCameraPosY, Game::kRedColor, L"CameraPos: %.2f, %.2f, %.2f", pos_.x_, pos_.y_, pos_.z_);
	DrawFormatString(0, kDebugTargetPosY, Game::kYellowColor, L"TargetPos: %.2f, %.2f, %.2f", targetPos.x_, targetPos.y_, targetPos.z_);
	DrawFormatString(0, kDebugdirectionVecPosY, Game::kGreenColor, L"DirVec: %.2f, %.2f, %.2f", directionVec.x_, directionVec.y_, directionVec.z_);
#endif

	//基底クラスの更新
	CameraBase::Update(stageModelHandle);
}
