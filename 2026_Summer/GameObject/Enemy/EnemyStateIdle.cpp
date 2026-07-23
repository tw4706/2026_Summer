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
	const float kArriveThreshold = 150.0f;

	//視線の高さ(Rayで障害物の判定を行うのに使用)
	const float kEyeHeight = 50.0f;

	//反応行動を開始する範囲
	constexpr float kSearchReactRange = 1000.0f;
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

	//次のwayPointのIDを取得
	int nextId = pLoader_->GetNextWayPointId(wayPoints, nearestId, -1);

	//敵の次に向かうwayPointのIdを更新
	enemy->nextWayPointId_ = nextId;
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//敵の位置とプレイヤーのPosを取得
	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//視線が通っているかの判定
	bool hasLineOfSight = HasLineOfSight(enemy->GetStageModelHandle(), enemyPos, playerPos);

	//プレイヤーが索敵範囲に入っているかつ視線が通っていたら(障害物にさえぎられていない)
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

	//wayPointの配列を取得
	const auto& wayPoints = enemy->pWayPointLoader_->GetWayPoints(enemy->areaId_);
	if (wayPoints.empty()) return;

	//次のターゲットIDを代入
	int targetId = enemy->nextWayPointId_;
	const WayPointLoader::WayPoint* pTargetWp = pLoader_->FindWayPointById(wayPoints, targetId);
	if (!pTargetWp) return;

	//デバッグ用に目標座標のセット
	enemy->debugNextPos_ = pTargetWp->pos;
	enemy->hasDebugTarget_ = true;

	//経路が無い時だけ直進判定をする
	if (!enemy->pathFollower_.HasPath())
	{
		bool canWalkDir = IsPathWalkable(enemy->pNaviGrid_, enemyPos, pTargetWp->pos);

		//次の経路に歩行できない場合
		if (!canWalkDir)
		{
			//経路を再検索
			std::vector<Vector3> path = enemy->pathFinder_.FindPath(enemyPos, pTargetWp->pos);

			//経路はない場合はさっき再検索した経路をセットする
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

	//wayPoint間の距離
	float wayPointDistance = toWayPoint.Length();

	//経路が無い状態かつWayPointに到達したら次の接続先へ切り替える
	bool isPathFinished = !enemy->pathFollower_.HasPath() || enemy->pathFollower_.IsPathFinished();

	//距離が到達しているかつ経路が終わっている場合
	if (wayPointDistance < kArriveThreshold && isPathFinished)
	{
		//次のIDを代入
		int currentId = targetId;

		//現在のIDを保存
		int prevId = enemy->currentWayPointId_;

		//敵の現在のwaypointIdを代入
		enemy->currentWayPointId_ = currentId;

		//次のIDを取得
		int nextId = pLoader_->GetNextWayPointId(wayPoints, currentId, prevId);

		//次のIdに代入
		enemy->nextWayPointId_ = nextId;

		//次のWayPointへ向かうため
		//敵が経路を持っている場合かつ敵が経路に到達していたら古い経路をクリア
		if (enemy->pathFollower_.HasPath() && enemy->pathFollower_.IsPathFinished())
		{
			enemy->pathFollower_.ClearPath();
		}
		return;
	}

	//正規化
	toTarget.Normalize();

	//速度・位置の適用
	ApplyMove(enemy, enemyPos, toTarget, kMoveSpeed);

	//角度を線形補間して適用
	enemy->moveAngle_ = RotateAngle(enemy->moveAngle_, toTarget, kRotateLerpRate);
}

void EnemyStateIdle::Exit()
{
}
