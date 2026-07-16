#include "EnemyStateReturn.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"
#include "WayPointLoader.h"
#include <cmath>
#include <limits>

namespace
{
	//移動速度
	const float kMoveSpeed = 0.5f;

	//線形補間の割合
	const float kRotateLerpRate = 0.3f;

	//復帰目標地点に到達したとみなす距離
	const float kArriveThreshold = 30.0f;

	//線分ab上で、posに最も近い点を求める
	Vector3 ClosestPointOnSegment(const Vector3& a, const Vector3& b, const Vector3& pos)
	{
		Vector3 ab = b - a;
		float abLenSq = ab.LengthSq();

		//始点と終点が同じ場合は始点を返す
		if (abLenSq <= 0.01f)
		{
			return a;
		}

		Vector3 ap = pos - a;
		float t = ap.Dot(ab) / abLenSq;

		//tを0から1の範囲にクランプ
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;

		return a + Vector3{ ab * t };
	}
}

EnemyStateReturn::EnemyStateReturn(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy, searchRadius)
{
}

void EnemyStateReturn::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//状態遷移
	enemy->ChangeAnimation(AnimationState::Walk);

	Vector3 enemyPos = enemy->GetPos();

	//デフォルトは現在地
	returnNextPos_ = enemyPos;
	fromWayPointId_ = -1;
	toWayPointId_ = -1;

	const auto& wayPoints = enemy->pWayPointLoader_->GetWayPoints(enemy->areaId_);
	if (wayPoints.empty()) return;

	float nearestDistSq = (std::numeric_limits<float>::max)();

	//ルートの中から、現在地に最も近い点を探す
	for (const auto& wp : wayPoints)
	{
		for (int connId : wp.connections)
		{
			//接続先のWayPointを検索
			const WayPointLoader::WayPoint* pTo = nullptr;
			for (const auto& wp2 : wayPoints)
			{
				if (wp2.id == connId)
				{
					pTo = &wp2;
					break;
				}
			}
			if (!pTo) continue;

			Vector3 closest = ClosestPointOnSegment(wp.pos, pTo->pos, enemyPos);
			Vector3 diff = closest - enemyPos;
			float distSq = diff.LengthSq();

			if (distSq < nearestDistSq)
			{
				nearestDistSq = distSq;
				returnNextPos_ = closest;
				fromWayPointId_ = wp.id;
				toWayPointId_ = connId;
			}
		}
	}
}

void EnemyStateReturn::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy) return;

	Vector3 enemyPos = enemy->GetPos();

	//敵から復帰目標地点までのベクトルを計算
	Vector3 toTarget = returnNextPos_ - enemyPos;
	toTarget.y_ = 0.0f;

	//距離の計算
	float distance = std::sqrt(toTarget.x_ * toTarget.x_ + toTarget.z_ * toTarget.z_);

	//復帰目標地点に到達したら巡回状態へ遷移する
	if (distance < kArriveThreshold)
	{
		//戻った区間の情報を敵にセットしてから遷移する
		if (fromWayPointId_ != -1 && toWayPointId_ != -1)
		{
			enemy->currentWayPointId_ = fromWayPointId_;
			enemy->nextWayPointId_ = toWayPointId_;
		}

		//Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	//正規化
	toTarget.Normalize();

	//速度・位置の適用
	ApplyMove(enemy, enemyPos, toTarget, kMoveSpeed);

	//角度を線形補間して適用
	enemy->moveAngle_ = RotateAngle(enemy->moveAngle_, toTarget, kRotateLerpRate);
}

void EnemyStateReturn::Exit()
{
}
