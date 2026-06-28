#include "AStarPathFinder.h"
#include "NavigationGrid.h"
#include <queue>
#include<cmath>

namespace
{
	//8方向移動(斜め含む)
	const int kDx[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };
	const int kDz[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };
	//斜め移動のコスト(√2)
	const float kDiagonalCost = 1.41421356f;
	//直進移動のコスト
	const float kStraightCost = 1.0f;
}

AStarPathFinder::AStarPathFinder()
{
}

void AStarPathFinder::SetNavigationGrid(const NavigationGrid* pNavGrid)
{
	pNaviGrid_ = pNavGrid;
}

AStarPathFinder::AStarPathFinder(const NavigationGrid* pNavGrid):
	pNaviGrid_(pNavGrid)
{
}

std::vector<Vector3> AStarPathFinder::FindPath(const Vector3& startPos, const Vector3& endPos) const
{
	std::vector<Vector3> resultPath;

	if (!pNaviGrid_)
	{
		return resultPath;
	}

	int startX, startZ, goalX, goalZ;
	pNaviGrid_->WorldPosToGrid(startPos, startX, startZ);
	pNaviGrid_->WorldPosToGrid(endPos, goalX, goalZ);

	const auto* startNode = pNaviGrid_->GetNode(startX, startZ);
	const auto* goalNode = pNaviGrid_->GetNode(goalX, goalZ);

	//範囲外、または歩行不可なら探索しない
	if (!startNode || !goalNode || !startNode->iswalked || !goalNode->iswalked)
	{
		return resultPath;
	}

	int width = pNaviGrid_->GetWidth();
	int height = pNaviGrid_->GetHeight();

	auto toIndex = [width](int x, int z) { return z * width + x; };

	//fCostが小さい順に取り出す優先度付きキュー
	auto cmp = [](const AStarNode& a, const AStarNode& b)
		{
			return a.fCost() > b.fCost();
		};
	std::priority_queue<AStarNode, std::vector<AStarNode>, decltype(cmp)> openList(cmp);

	std::vector<float> bestGCost(width * height, -1.0f);
	std::vector<bool> closed(width * height, false);
	std::vector<int> parentX(width * height, -1);
	std::vector<int> parentZ(width * height, -1);

	AStarNode start;
	start.x = startX;
	start.z = startZ;
	start.gCost = 0.0f;
	start.hCost = Heuristic(startX, startZ, goalX, goalZ);
	openList.push(start);
	bestGCost[toIndex(startX, startZ)] = 0.0f;

	bool found = false;

	while (!openList.empty())
	{
		AStarNode current = openList.top();
		openList.pop();

		int currentIndex = toIndex(current.x, current.z);

		//既にクローズ済み(古い情報)ならスキップ
		if (closed[currentIndex])
		{
			continue;
		}
		closed[currentIndex] = true;

		//ゴールに到達
		if (current.x == goalX && current.z == goalZ)
		{
			found = true;
			break;
		}

		//8方向探索
		for (int i = 0; i < 8; ++i)
		{
			int nx = current.x + kDx[i];
			int nz = current.z + kDz[i];

			const auto* neighbor = pNaviGrid_->GetNode(nx, nz);
			if (!neighbor || !neighbor->iswalked)
			{
				continue;
			}

			int neighborIndex = toIndex(nx, nz);
			if (closed[neighborIndex])
			{
				continue;
			}

			bool isDiagonal = (kDx[i] != 0 && kDz[i] != 0);

			//斜め移動の場合、両脇が歩行不可だと角を斬って通れないようにする
			if (isDiagonal)
			{
				const auto* sideA = pNaviGrid_->GetNode(current.x + kDx[i], current.z);
				const auto* sideB = pNaviGrid_->GetNode(current.x, current.z + kDz[i]);
				if ((!sideA || !sideA->iswalked) && (!sideB || !sideB->iswalked))
				{
					continue;
				}
			}

			float moveCost = isDiagonal ? kDiagonalCost : kStraightCost;
			float newGCost = current.gCost + moveCost;

			if (bestGCost[neighborIndex] < 0.0f || newGCost < bestGCost[neighborIndex])
			{
				bestGCost[neighborIndex] = newGCost;
				parentX[neighborIndex] = current.x;
				parentZ[neighborIndex] = current.z;

				AStarNode next;
				next.x = nx;
				next.z = nz;
				next.gCost = newGCost;
				next.hCost = Heuristic(nx, nz, goalX, goalZ);
				openList.push(next);
			}
		}
	}

	if (!found)
	{
		return resultPath;
	}

	//ゴールから親をたどって経路を逆順に構築
	std::vector<Vector3> reversedPath;
	int traceX = goalX;
	int traceZ = goalZ;

	while (!(traceX == startX && traceZ == startZ))
	{
		const auto* node = pNaviGrid_->GetNode(traceX, traceZ);
		reversedPath.push_back(node->pos);

		int index = toIndex(traceX, traceZ);
		int px = parentX[index];
		int pz = parentZ[index];
		traceX = px;
		traceZ = pz;
	}

	const auto* startNodeForPath = pNaviGrid_->GetNode(startX, startZ);
	reversedPath.push_back(startNodeForPath->pos);

	//逆順なのでスタート→ゴール順に反転
	resultPath.assign(reversedPath.rbegin(), reversedPath.rend());

	return resultPath;
}

float AStarPathFinder::Heuristic(int x1, int z1, int x2, int z2) const
{
	//斜め移動を考慮したユークリッド距離(マス単位)
	float dx = static_cast<float>(x2 - x1);
	float dz = static_cast<float>(z2 - z1);
	return std::sqrt(dx * dx + dz * dz);
}
