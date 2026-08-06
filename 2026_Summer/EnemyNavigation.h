#pragma once
#include "Math/Vector3.h"
#include "AStarPathFinder.h"
#include "PathFollower.h"
#include "CSV/WayPointLoader.h"

class NavigationGrid;
class EnemyNavigation
{
public:
	EnemyNavigation()=default;
	~EnemyNavigation()=default;

	/// <summary>
	/// ナビゲーショングリッドのセット
	/// </summary>
	/// <param name="pNavGrid">ナビゲーショングリッドのポインタ</param>
	void SetNavigationGrid(const NavigationGrid* pNavGrid);

	/// <summary>
	/// ナビゲーショングリッドの取得
	/// </summary>
	/// <returns></returns>
	const NavigationGrid*GetNavigationGrid()const { return pNavGrid_; }

	/// <summary>
	/// WayPointLoaderの設定・取得
	/// </summary>
	void SetWayPointLoader(const WayPointLoader* pLoader) { pWayPointLoader_ = pLoader; }
	const WayPointLoader* GetWayPointLoader() const { return pWayPointLoader_; }

	/// <summary>
	/// エリアIDの設定・取得
	/// </summary>
	void SetAreaId(int areaId) { areaId_ = areaId; }
	int GetAreaId() const { return areaId_; }

	/// <summary>
	/// 経路探索・パスフォロワーの取得(
	/// </summary>
	AStarPathFinder& GetPathFinder() { return pathFinder_; }
	PathFollower& GetPathFollower() { return pathFollower_; }

	/// <summary>
	/// 現在/目標WayPointIDの設定・取得
	/// </summary>
	void SetCurrentWayPointId(int id) { currentWayPointId_ = id; }
	int GetCurrentWayPointId() const { return currentWayPointId_; }

	void SetNextWayPointId(int id) { nextWayPointId_ = id; }
	int GetNextWayPointId() const { return nextWayPointId_; }

	/// <summary>
	/// 再検索するまでのタイマーの取得
	/// </summary>
	/// <returns>タイマーを返す</returns>
	float GetPathRetryTimer() const { return pathRetryTimer_; }

	/// <summary>
	/// 再検索するまでのタイマーのセット
	/// </summary>
	/// <param name="time">時間</param>
	void SetPathRetryTimer(float time) { pathRetryTimer_ = time; }

	/// <summary>
	/// デバッグ用の目標座標の設定・取得
	/// </summary>
	void SetDebugTarget(const Vector3& pos);
	void ClearDebugTarget() { hasDebugTarget_ = false; }
	bool HasDebugTarget() const { return hasDebugTarget_; }
	const Vector3& GetDebugNextPos() const { return debugNextPos_; }

private:
	//経路探索のA*アルゴリズムとwaypointの管理
	AStarPathFinder pathFinder_;
	//パスフォロワー
	PathFollower pathFollower_;
	//ナビゲーショングリッド
	const NavigationGrid* pNavGrid_ = nullptr;
	//WayPointLoaderの参照
	const WayPointLoader* pWayPointLoader_ = nullptr;
	//エリアID
	int areaId_ = 0;
	//現在いるWayPointのID
	int currentWayPointId_ = -1;
	//目標のWayPointのID
	int nextWayPointId_ = -1;

	//デバッグ描画用の目標までの座標
	Vector3 debugNextPos_ = { 0.0f, 0.0f, 0.0f };
	bool hasDebugTarget_ = false;

	//経路の探索を再検索するまでの時間
	float pathRetryTimer_ = 0.0f;
};

