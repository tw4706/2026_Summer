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

	//指定エリア内で指定座標に最も近いWayPointのidを返す
	int FindNearestWayPointId(const std::vector<WayPointLoader::WayPoint>& wayPoints, const Vector3& pos)
	{
		int nearestId = -1;
		float nearestDistSq = (std::numeric_limits<float>::max)();

		for (const auto& wp : wayPoints)
		{
			Vector3 diff = wp.pos - pos;
			float distSq = diff.LengthSq();

			if (distSq < nearestDistSq)
			{
				nearestDistSq = distSq;
				nearestId = wp.id;
			}
		}

		return nearestId;
	}

	//idからWayPointを検索する
	const WayPointLoader::WayPoint* FindWayPointById(const std::vector<WayPointLoader::WayPoint>& wayPoints, int id)
	{
		for (const auto& wp : wayPoints)
		{
			if (wp.id == id)
			{
				return &wp;
			}
		}
		return nullptr;
	}

	//現在のWayPointから次のWayPointを決定する
	int GetNextWayPointId(const std::vector<WayPointLoader::WayPoint>& wayPoints, int currentId, int nextId)
	{
		//現在のWayPointの情報を取得
		const WayPointLoader::WayPoint* pCurrent = FindWayPointById(wayPoints, currentId);
		if (!pCurrent || pCurrent->connections.empty())
		{
			return -1;
		}

		if (pCurrent->connections.size() == 1)
		{
			return pCurrent->connections.front();
		}

		for (int connID : pCurrent->connections)
		{
			if (connID != nextId)
			{
				//次に進むべきコネクト先のIDを返す
				return connID;
			}
		}

		//そうでない場合は前のIDを返す
		return pCurrent->connections.front();;
	}
}


EnemyStateIdle::EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy, searchRadius)
{
}

void EnemyStateIdle::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Walk);

	const WayPointLoader* pLoader = enemy->GetWayPointLoader();
	if (!pLoader) return;

	const auto& wayPoints = pLoader->GetWayPoints(enemy->GetAreaId());
	if (wayPoints.empty()) return;

	//すでに有効な巡回情報を持っている場合はそのまま引き継ぐ
	bool hasValidCurrent = FindWayPointById(wayPoints, enemy->GetCurrentWayPointId()) != nullptr;
	bool hasValidTarget = FindWayPointById(wayPoints, enemy->GetNextWayPointId()) != nullptr;
	if (hasValidCurrent && hasValidTarget)
	{
		return;
	}

	//巡回情報が無い場合は最寄りのWayPointから巡回を開始する
	int nearestId = FindNearestWayPointId(wayPoints, enemy->GetPos());
	enemy->SetCurrentWayPointId(nearestId);

	int nextId = GetNextWayPointId(wayPoints, nearestId, -1);
	enemy->SetNextWayPointId(nextId);
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//索敵範囲かつまだ当たっていなかったら
	if (PlayerSearchDistance(searchRadius_))
	{
		//Run状態へ遷移
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	const WayPointLoader* pLoader = enemy->GetWayPointLoader();
	if (!pLoader) return;

	const auto& wayPoints = pLoader->GetWayPoints(enemy->GetAreaId());
	if (wayPoints.empty()) return;

	int targetId = enemy->GetNextWayPointId();
	const WayPointLoader::WayPoint* pTargetWp = FindWayPointById(wayPoints, targetId);
	if (!pTargetWp) return;

	Vector3 enemyPos = enemy->GetPos();

	//デバッグ用に目標座標のセット
	enemy->SetDebugNextPos(pTargetWp->pos);
	enemy->SetHasDebugTarget(true);

	//敵から目標WayPointまでのベクトルを計算
	Vector3 toTarget = pTargetWp->pos - enemyPos;
	toTarget.y_ = 0.0f;

	//距離の計算
	float distance = toTarget.Length();

	//目標WayPointに到達したら、次の接続先WayPointへ目標を切り替える
	if (distance < kArriveThreshold)
	{
		int arrivedId = targetId;
		int oldCurrentId = enemy->GetCurrentWayPointId();

		enemy->SetCurrentWayPointId(arrivedId);

		int nextId = GetNextWayPointId(wayPoints, arrivedId, oldCurrentId);
		enemy->SetNextWayPointId(nextId);
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
	float currentAngle = enemy->GetMoveAngle();

	float angleDiff = targetAngle - currentAngle;

	//差分を-πからπの範囲に正規化
	while (angleDiff > DX_PI_F)  angleDiff -= 2.0f * DX_PI_F;
	while (angleDiff < -DX_PI_F) angleDiff += 2.0f * DX_PI_F;

	//線形補間を用いて滑らかに回転する
	float nextAngle = currentAngle + angleDiff * kRotateLerpRate;

	//計算した角度を適用
	enemy->SetMoveAngle(targetAngle);
}

void EnemyStateIdle::Exit()
{
}
