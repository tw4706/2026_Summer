#pragma once
#include "Math/Vector3.h"
#include <vector>
#include <string>
#include <unordered_map>

class WayPointLoader
{
public:

	struct WayPoint
	{
		int id = -1;
		Vector3 pos;
		std::vector<int> connections; //接続しているほかのWayPointのid
	};

	/// <summary>
	/// WayPointのデータをCSVから読み込む
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>読み込み成功はtrue,失敗はfalse</returns>
	bool Load(const std::wstring& path);

	std::vector<Vector3> FindPath(int areaId, const Vector3& start, const Vector3& goal) const;

	/// <summary>
	/// WayPointのデータをCSVに書いているエリアごとのIDで取得
	/// </summary>
	/// <param name="areaId">エリアID</param>
	/// <returns></returns>
	const std::vector<WayPoint>& GetWayPoints(int areaId)const;

	/// <summary>
	/// 指定エリア内で指定座標に最も近いWayPointのidを返す
	/// </summary>
	int FindNearestWayPointId(const std::vector<WayPoint>& wayPoints, const Vector3& pos);

	/// <summary>
	/// idからWayPointを検索する
	/// </summary>
	/// <param name="wayPoints">waypoint(各地点のポイントとなる場所)</param>
	/// <param name="id">エリアID</param>
	/// <returns>wayPointのidを返す</returns>
	const WayPoint* FindWayPointById(const std::vector<WayPoint>& wayPoints, int id);

	/// <summary>
	/// 現在のWayPointから次のWayPointを決定する
	/// </summary>
	/// <param name="wayPoints">waypoint(各地点のポイントとなる場所)</param>
	/// <param name="currentId">現在のエリアID</param>
	/// <param name="nextId">次のエリアID</param>
	/// <returns></returns>
	int GetNextWayPointId(const std::vector<WayPointLoader::WayPoint>& wayPoints, int currentId, int nextId);

private:
	//指定エリア内で指定座標に最も近いWayPointのidを返す
	int GetNearestWayPointId(int areaId, const Vector3& pos) const;

	//コンマ区切りの文字列をトークンに分割する
	std::vector<std::string> Split(const std::string& str, char delimiter) const;

	std::unordered_map<int, std::vector<WayPoint>> wayPointsByArea_;
};

