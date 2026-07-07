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
}

void LockOnCamera::Update(int stageModelHandle)
{
	auto player = pPlayer_.lock();
	auto enemy = pEnemy_.lock();

	//プレイヤーまたは敵が存在しない場合は何もしない
	if (!player || !enemy)return;

	//敵のターゲット座標を取得
	Vector3 enemyTargetPos = enemy->GetPos();

	//カメラのターゲット座標に代入
	cameraTarget_ = enemyTargetPos;

	Vector3 playerCameraPos = player->GetCameraTarget();

	//プレイヤーから敵へ向かう方向ベクトルを作成
	Vector3 directionVec = cameraTarget_ - playerCameraPos;

	directionVec.y_ = 0.0f;

	//正規化して視線方向のベクトルを作成
	directionVec.Normalize();

	//ロックオン時のカメラのターゲット座標の計算
	Vector3 targetPos = playerCameraPos - (directionVec * 300.0f);

	//線形補間を行い、滑らかな追従を行う
	pos_ = Vector3::Lerp(pos_, targetPos, 0.3f);

	//カメラとステージの消灯判定を行う
	pos_ = CheckCollCameraToStage(stageModelHandle, playerCameraPos, pos_);

	//基底クラスの更新
	CameraBase::Update(stageModelHandle);
}
