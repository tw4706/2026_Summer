#include "EnemyNavigation.h"

void EnemyNavigation::SetNavigationGrid(const NavigationGrid* pNavGrid)
{
	pNavGrid_ = pNavGrid;
	pathFinder_.SetNavigationGrid(pNavGrid);
}

void EnemyNavigation::SetDebugTarget(const Vector3& pos)
{
	debugNextPos_ = pos;
	hasDebugTarget_ = true;
}
