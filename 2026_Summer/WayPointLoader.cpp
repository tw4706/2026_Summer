#include "WayPointLoader.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <limits>


bool WayPointLoader::Load(const std::wstring& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return false;
	}

	wayPointsByArea_.clear();

	std::string line;
	//1行目はヘッダーなので読み飛ばす
	std::getline(file, line);

	while (std::getline(file, line))
	{
		if (line.empty()) continue;

		//"" で囲まれたconnections部分にカンマが含まれるため、
		//簡易的にダブルクォートの中身を別扱いで処理する
		std::vector<std::string> tokens;
		std::string token;
		bool inQuotes = false;

		for (char c : line)
		{
			if (c == '"')
			{
				inQuotes = !inQuotes;
			}
			else if (c == ',' && !inQuotes)
			{
				tokens.push_back(token);
				token.clear();
			}
			else
			{
				token += c;
			}
		}
		tokens.push_back(token);

		if (tokens.size() < 6) continue;

		WayPoint wp;
		int areaId = std::stoi(tokens[0]);
		wp.id = std::stoi(tokens[1]);
		wp.pos.x_ = std::stof(tokens[2]) * 100.0f;
		wp.pos.y_ = std::stof(tokens[3]) * 100.0f;
		wp.pos.z_ = std::stof(tokens[4]) * -100.0f;

		//connectionsをパース
		std::string connStr = tokens[5];
		if (!connStr.empty())
		{
			auto connTokens = Split(connStr, ',');
			for (const auto& c : connTokens)
			{
				if (!c.empty())
				{
					wp.connections.push_back(std::stoi(c));
				}
			}
		}

		wayPointsByArea_[areaId].push_back(wp);
	}

#ifdef _DEBUG
	for (const auto& pair : wayPointsByArea_) {
		int count = pair.second.size();
		char buf[256];
		sprintf_s(buf, "--- [Debug] Area %d に %d 個のWayPointをロードしました ---\n", pair.first, count);
		OutputDebugStringA(buf);
	}
#endif

	return true;
}

std::vector<Vector3> WayPointLoader::FindPath(int areaId, const Vector3& start, const Vector3& goal) const
{
	auto it = wayPointsByArea_.find(areaId);
	if (it == wayPointsByArea_.end()) return {};

	const auto& wayPoints = it->second;

	int startId = GetNearestWayPointId(areaId, start);
	int goalId = GetNearestWayPointId(areaId, goal);

	if (startId == -1 || goalId == -1) return {};

	//id -> WayPointの参照を引けるようにする
	std::unordered_map<int, const WayPoint*> idToWp;
	for (const auto& wp : wayPoints)
	{
		idToWp[wp.id] = &wp;
	}

	//BFSで経路探索
	std::queue<int> q;
	std::unordered_map<int, int> cameFrom;
	std::unordered_map<int, bool> visited;

	q.push(startId);
	visited[startId] = true;

	while (!q.empty())
	{
		int current = q.front();
		q.pop();

		if (current == goalId) break;

		auto wpIt = idToWp.find(current);
		if (wpIt == idToWp.end()) continue;

		for (int next : wpIt->second->connections)
		{
			if (!visited[next])
			{
				visited[next] = true;
				cameFrom[next] = current;
				q.push(next);
			}
		}
	}

	if (!visited[goalId]) return {};

	//ゴールからスタートへ逆順に辿って経路を復元
	std::vector<Vector3> path;
	int node = goalId;

	while (node != startId)
	{
		path.push_back(idToWp[node]->pos);

		auto fromIt = cameFrom.find(node);
		if (fromIt == cameFrom.end()) break; //安全対策
		node = fromIt->second;
	}
	path.push_back(idToWp[startId]->pos);

	std::reverse(path.begin(), path.end());
	path.push_back(goal); //最後にプレイヤーの実座標を追加

	return path;
}

const std::vector<WayPointLoader::WayPoint>& WayPointLoader::GetWayPoints(int areaId) const
{
	const std::vector<WayPoint>emptyWP;

	auto it = wayPointsByArea_.find(areaId);

	if (it == wayPointsByArea_.end())
	{
		return emptyWP;
	}

	return it->second;
}

int WayPointLoader::GetNearestWayPointId(int areaId, const Vector3& pos) const
{
	auto it = wayPointsByArea_.find(areaId);
	if (it == wayPointsByArea_.end()) return -1;

	int nearestId = -1;
	float nearestDistSq = (std::numeric_limits<float>::max)();

	for (const auto& wp : it->second)
	{
		Vector3 diff = wp.pos - pos;
		float distSq = diff.x_ * diff.x_ + diff.y_ * diff.y_ + diff.z_ * diff.z_;

		if (distSq < nearestDistSq)
		{
			nearestDistSq = distSq;
			nearestId = wp.id;
		}
	}

	return nearestId;
}

std::vector<std::string> WayPointLoader::Split(const std::string& str, char delimiter) const
{
	std::vector<std::string> result;
	std::stringstream ss(str);
	std::string item;

	while (std::getline(ss, item, delimiter))
	{
		result.push_back(item);
	}

	return result;
}
