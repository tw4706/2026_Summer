#include "EnemyStateIdle.h"
#include "EnemyStateReact.h"
#include "EnemyStateJump.h"
#include "EnemyStateAttack.h"
#include "EnemyBase.h"
#include "System/NavigationGrid.h"

namespace
{
	//移動速度
	const float kMoveSpeed = 0.3f;

	//線形補間の割合
	const float kRotateLerpRate = 0.3f;

	//WayPointに到達したとみなす距離
	const float kArriveThreshold = 50.0f;

	//視線の高さ(Rayで障害物の判定を行うのに使用)
	const float kEyeHeight = 50.0f;

	//段差を検知する用の前方向のチェックする距離
	const float kJumpDistance = 60.0f;

	//跳び越え可能と判断する高低差
	const float kMinJumpableHeight = 20.0f;
	const float kMaxJumpableHeight = 100.0f;

	//反応行動を開始する範囲
	constexpr float kSearchReactRange = 700.0f;

	//指定座標の真下にレイを飛ばして地面の高さを取得する
	//地面にhitしなければfalseを返す
	bool GetGroundHeight(int stageModelHandle, const Vector3& pos, float& outHeight)
	{
		VECTOR start = VGet(pos.x_, pos.y_ + 500.0f, pos.z_);
		VECTOR end = VGet(pos.x_, pos.y_ - 500.0f, pos.z_);

		MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageModelHandle, -1, start, end);
		if (!hit.HitFlag) return false;

		outHeight = hit.HitPosition.y;
		return true;
	}

	bool TryDetectJumpableStep(int stageModelHandle, const Vector3& from, const Vector3& moveDir, Vector3& landingPos)
	{
		Vector3 dir = moveDir;
		dir.y_ = 0.0f;
		if (dir.Length() <= 0.0001f) return false;
		dir.Normalize();

		float currentHeight = 0.0f;
		if (!GetGroundHeight(stageModelHandle, from, currentHeight)) return false;

		Vector3 probePos = from + dir * kJumpDistance;

		float probeHeight = 0.0f;
		if (!GetGroundHeight(stageModelHandle, probePos, probeHeight)) return false;

		float heightDiff = probeHeight - currentHeight;
		if (heightDiff < kMinJumpableHeight || heightDiff > kMaxJumpableHeight) return false;

		landingPos = Vector3(probePos.x_, probeHeight, probePos.z_);
		return true;
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

	//視線が通っている場合は経路追従をやめて直進する
	if (enemy->pathFollower_.HasPath())
	{
		enemy->pathFollower_.ClearPath();
	}

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

	if (enemy->IsPlayerInRange(kSearchReactRange)&& hasLineOfSight)
	{
		//状態を遷移する前に経路をクリア
		if (enemy->pathFollower_.HasPath())
		{
			enemy->pathFollower_.ClearPath();
		}

		//反応している状態へ遷移
		auto nextState = std::make_shared<EnemyStateReact>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	const auto& wayPoints = enemy->pWayPointLoader_->GetWayPoints(enemy->areaId_);
	if (wayPoints.empty()) return;

	int targetId = enemy->nextWayPointId_;
	const WayPointLoader::WayPoint* pTargetWp = pLoader_->FindWayPointById(wayPoints, targetId);
	if (!pTargetWp) return;

	//デバッグ用に目標座標のセット
	enemy->debugNextPos_ = pTargetWp->pos;
	enemy->hasDebugTarget_ = true;

	//経路が無い時だけ直進判定をする
	if (!enemy->pathFollower_.HasPath())
	{
		bool canWalkDirect = IsPathWalkable(enemy->pNaviGrid_, enemyPos, pTargetWp->pos);

		if (!canWalkDirect)
		{
			std::vector<Vector3> path = enemy->pathFinder_.FindPath(enemyPos, pTargetWp->pos);
			if (!path.empty())
			{
				enemy->pathFollower_.SetPath(path);
			}
		}
	}

	//経路上の現在の目標地点を取得
	Vector3 moveTargetPos = enemy->pathFollower_.HasPath()
		? enemy->pathFollower_.GetCurrentTarget(enemyPos)
		: pTargetWp->pos;

	//敵から目標WayPointまでのベクトルを計算
	Vector3 toTarget = moveTargetPos - enemyPos;
	toTarget.y_ = 0.0f;

	//敵から目標WayPointまでのベクトルを計算
	Vector3 toWayPoint = pTargetWp->pos - enemyPos;
	toWayPoint.y_ = 0.0f;

	float wayPointDistance = toWayPoint.Length();

	//経路が無い状態かつWayPointに到達したら次の接続先へ切り替える
	bool pathFinishedOrNone = !enemy->pathFollower_.HasPath() || enemy->pathFollower_.IsPathFinished();

	if (wayPointDistance < kArriveThreshold && pathFinishedOrNone)
	{
		int arrivedId = targetId;
		int oldCurrentId = enemy->currentWayPointId_;

		enemy->currentWayPointId_ = arrivedId;

		int nextId = pLoader_->GetNextWayPointId(wayPoints, arrivedId, oldCurrentId);
		enemy->nextWayPointId_ = nextId;

		//次のWayPointへ向かうため古い経路をクリア
		if (enemy->pathFollower_.HasPath())
		{
			enemy->pathFollower_.ClearPath();
		}
		return;
	}

	//正規化
	toTarget.Normalize();

	Vector3 landingPos;
	//着地地点を保存しジャンプ状態に遷移
	if (TryDetectJumpableStep(enemy->GetStageModelHandle(), enemyPos, toTarget, landingPos))
	{
		enemy->jumpTargetPos_ = landingPos;

		auto nextState = std::make_shared<EnemyStateJump>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}

	//速度・位置の適用
	ApplyMove(enemy, enemyPos, toTarget, kMoveSpeed);

	//角度を線形補間して適用
	enemy->moveAngle_ = RotateAngle(enemy->moveAngle_, toTarget, kRotateLerpRate);
}

void EnemyStateIdle::Exit()
{
}
