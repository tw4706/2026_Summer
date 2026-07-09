#include "LockOnCamera.h"
#include "Enemy/EnemyBase.h"
#include "Player/Player.h"

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
	Vector3 enemyTargetPos = enemy->GetPos();

	//カメラのターゲット座標に代入
	cameraTarget_ = enemyTargetPos;

	Vector3 playerCameraPos = player->GetCameraTarget();

	//プレイヤーから敵へ向かう方向ベクトルを作成
	Vector3 directionVec = playerCameraPos - cameraTarget_;

	directionVec.y_ = 0.0f;

	//正規化して視線方向のベクトルを作成
	const float kMinDirLengthSq = 1.0f;
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
	Vector3 targetPos = playerCameraPos + (directionVec * 300.0f) + Vector3{ 0.0f,100.0f,50.0f };

	//線形補間を行い、滑らかな追従を行う
	pos_ = Vector3::Lerp(pos_, targetPos, 0.3f);

	//カメラとステージの消灯判定を行う
	pos_ = CheckCollCameraToStage(stageModelHandle, playerCameraPos, pos_);

	DrawFormatString(0, 100, GetColor(255, 0, 0), L"CameraPos: %.2f, %.2f, %.2f", pos_.x_, pos_.y_, pos_.z_);
	DrawFormatString(0, 130, GetColor(255, 255, 0), L"TargetPos: %.2f, %.2f, %.2f", targetPos.x_, targetPos.y_, targetPos.z_);
	DrawFormatString(0, 160, GetColor(0, 255, 0), L"DirVec: %.2f, %.2f, %.2f", directionVec.x_, directionVec.y_, directionVec.z_);

	//基底クラスの更新
	CameraBase::Update(stageModelHandle);
}
