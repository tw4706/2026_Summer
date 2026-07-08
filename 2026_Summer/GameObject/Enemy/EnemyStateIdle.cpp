#include "EnemyStateIdle.h"
#include "EnemyStateRun.h"
#include "EnemyBase.h"
namespace
{
	//移動速度
	const float kMoveSpeed = 0.3f;

	//経過時間
	const float kDeltaTime = 1.0f / 60.0f;

	//線形補間の割合
	const float kRotateLerpRate = 0.3f;

	//WayPointに到達したとみなす距離
	const float kArriveThreshold = 50.0f;

	//視線の高さ(Rayで障害物の判定を行うのに使用)
	const float kEyeHeight = 50.0f;

	//敵の視線の先に障害物があるかどうかをレイを飛ばして判定する
	//hitしていなければ視線が通っている(=true)
	bool HasLineOfSight(int stageModelHandle, const Vector3& from, const Vector3& to)
	{
		VECTOR start = VGet(from.x_, from.y_ + kEyeHeight, from.z_);
		VECTOR end = VGet(to.x_, to.y_ + kEyeHeight, to.z_);

		MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageModelHandle, -1, start, end);

		return hit.HitFlag == false;
	}
}


EnemyStateIdle::EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy, searchRadius)
{
	pLoader_ = std::make_shared<WayPointLoader>();
}

void EnemyStateIdle::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Walk);

	const WayPointLoader* pLoader = enemy->pWayPointLoader_;
	if (!pLoader) return;

	const auto& wayPoints = pLoader->GetWayPoints(enemy->areaId_);
	if (wayPoints.empty()) return;

	//すでに有効な巡回情報を持っている場合はそのまま引き継ぐ
	bool hasValidCurrent = pLoader_->FindWayPointById(wayPoints, enemy->currentWayPointId_) != nullptr;
	bool hasValidTarget = pLoader_->FindWayPointById(wayPoints, enemy->nextWayPointId_) != nullptr;
	if (hasValidCurrent && hasValidTarget)
	{
		return;
	}

	//巡回情報が無い場合は最寄りのWayPointから巡回を開始する
	int nearestId = pLoader_->FindNearestWayPointId(wayPoints, enemy->GetPos());
	enemy->currentWayPointId_ = nearestId;

	int nextId = pLoader_->GetNextWayPointId(wayPoints, nearestId, -1);
	enemy->nextWayPointId_ = nextId;
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();
	bool hasLineOfSight = HasLineOfSight(enemy->GetStageModelHandle(), enemyPos, playerPos);

	if (PlayerSearchDistance(searchRadius_) && hasLineOfSight)
	{
		//Run状態へ遷移
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	const WayPointLoader* pLoader = enemy->pWayPointLoader_;
	if (!pLoader) return;

	const auto& wayPoints = pLoader->GetWayPoints(enemy->areaId_);
	if (wayPoints.empty()) return;

	int targetId = enemy->nextWayPointId_;
	const WayPointLoader::WayPoint* pTargetWp = pLoader_->FindWayPointById(wayPoints, targetId);
	if (!pTargetWp) return;

	//デバッグ用に目標座標のセット
	enemy->debugNextPos_ = pTargetWp->pos;
	enemy->hasDebugTarget_ = true;

	//敵から目標WayPointまでのベクトルを計算
	Vector3 toTarget = pTargetWp->pos - enemyPos;
	toTarget.y_ = 0.0f;

	//距離の計算
	float distance = toTarget.Length();

	//目標WayPointに到達したら、次の接続先WayPointへ目標を切り替える
	if (distance < kArriveThreshold)
	{
		int arrivedId = targetId;
		int oldCurrentId = enemy->currentWayPointId_;

		enemy->currentWayPointId_ = arrivedId;

		int nextId = pLoader_->GetNextWayPointId(wayPoints, arrivedId, oldCurrentId);
		enemy->nextWayPointId_ = nextId;
		return;
	}

	//正規化
	toTarget.Normalize();

	//移動速度を設定
	Vector3 moveVec = { toTarget.x_ * kMoveSpeed * kDeltaTime, 0.0f, toTarget.z_ * kMoveSpeed * kDeltaTime };
	//計算した位置を適用
	Vector3 nextPos = enemyPos + moveVec;
	//位置のセット
	enemy->SetPos(nextPos);

	//進行方向の角度
	float targetAngle = std::atan2f(toTarget.x_, -toTarget.z_);
	//現在の角度
	float currentAngle = enemy->moveAngle_;

	float angleDiff = targetAngle - currentAngle;

	//差分を-πからπの範囲に正規化
	while (angleDiff > DX_PI_F)  angleDiff -= 2.0f * DX_PI_F;
	while (angleDiff < -DX_PI_F) angleDiff += 2.0f * DX_PI_F;

	//線形補間を用いて滑らかに回転する
	float nextAngle = currentAngle + angleDiff * kRotateLerpRate;

	//計算した角度を適用
	enemy->moveAngle_ = nextAngle;
}

void EnemyStateIdle::Exit()
{
}
