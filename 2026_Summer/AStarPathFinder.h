#pragma once
#include"Math/Vector3.h"
#include<vector>


class NavigationGrid;
class AStarPathFinder
{
public:

	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	AStarPathFinder();

	AStarPathFinder(const NavigationGrid* pNavGrid);

	void SetNavigationGrid(const NavigationGrid* pNavGrid);

	std::vector<Vector3>FindPath(const Vector3& startPos, const Vector3& endPos)const;

private:

	struct AStarNode
	{
		int x = 0;
		int z = 0;
		float gCost = 0.0f;
		float hCost = 0.0f;

		float fCost()const { return gCost + hCost; }
	};

	float Heuristic(int x1, int z1, int x2, int z2)const;

	const NavigationGrid* pNaviGrid_ = nullptr;
};

