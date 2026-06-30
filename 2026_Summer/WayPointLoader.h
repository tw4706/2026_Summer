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
		std::vector<int> connections; // 接続している他WayPointのid
	};

	bool LoadWayPoint(const std::wstring& path);

	std::vector<Vector3> FindPath(int areaId, const Vector3& start, const Vector3& goal) const;

private:
	//指定エリア内で指定座標に最も近いWayPointのidを返す
	int GetNearestWayPointId(int areaId, const Vector3& pos) const;

	//カンマ区切りの文字列をトークンに分割する
	std::vector<std::string> Split(const std::string& str, char delimiter) const;

	std::unordered_map<int, std::vector<WayPoint>> wayPointsByArea_;
};

