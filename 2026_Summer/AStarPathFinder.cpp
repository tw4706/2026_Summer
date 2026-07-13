#include "AStarPathFinder.h"
#include "NavigationGrid.h"
#include <queue>
#include<cmath>

namespace
{
	//8方向移動
	//これによってXZ座標を合わせることで
	//右,左,上,下.右上,右下,左上,左下の移動を再現
	const int kDirectionX[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };
	const int kDirectionZ[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };

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

AStarPathFinder::AStarPathFinder(const NavigationGrid* pNavGrid) :
	pNaviGrid_(pNavGrid)
{
}

std::vector<Vector3> AStarPathFinder::FindPath(const Vector3& startPos, const Vector3& endPos) const
{
	//パスの結果を保存するための変数
	std::vector<Vector3> resultPath;

	//ナビゲーショングリッドが存在しない場合は現時点でのパスの結果を返す
	if (!pNaviGrid_)
	{
		return resultPath;
	}

	//グリッドの始点と終点を3D座標からグリッド座標である二次元座標に変換
	int startX, startZ, goalX, goalZ;
	pNaviGrid_->WorldPosToGrid(startPos, startX, startZ);
	pNaviGrid_->WorldPosToGrid(endPos, goalX, goalZ);

	//ノードの始点・終点
	const auto* startNode = pNaviGrid_->GetNode(startX, startZ);
	const auto* goalNode = pNaviGrid_->GetNode(goalX, goalZ);

	//範囲外か歩行できない場所なら探索をおこなわない
	if (!startNode || !goalNode || !startNode->iswalked || !goalNode->iswalked)
	{
		return resultPath;
	}

	int width = pNaviGrid_->GetWidth();
	int height = pNaviGrid_->GetHeight();

	//二次元座標を一次元のインデックスに変換するラムダ式
	auto toIndex = [width](int x, int z) { return z * width + x; };

	//fCostが小さい順に取り出す優先度付きキュー
	auto cmp = [](const AStarNode& a, const AStarNode& b)
		{
			return a.fCost() > b.fCost();
		};
	std::priority_queue<AStarNode, std::vector<AStarNode>, decltype(cmp)> openList(cmp);

	//各グリッドのすべての中で最小の移動コストを保存する配列
	std::vector<float> bestGCost(width * height, -1.0f);

	//経路探索が終了したマスを記録しておく配列
	std::vector<bool> closed(width * height, false);

	//経路を戻す際にどのマスから移動してきたかを記録する配列
	std::vector<int> parentX(width * height, -1);
	std::vector<int> parentZ(width * height, -1);

	//始点をリストに追加
	AStarNode start;
	start.x = startX;
	start.z = startZ;
	start.gCost = 0.0f;
	start.hCost = Heuristic(startX, startZ, goalX, goalZ);
	openList.push(start);
	bestGCost[toIndex(startX, startZ)] = 0.0f;

	//ゴールが見つかっているかどうかのフラグ
	bool isFoundGoal = false;

	while (!openList.empty())
	{
		//一番小さいかつゴールに近いノードを取り出す
		AStarNode current = openList.top();
		openList.pop();

		int currentIndex = toIndex(current.x, current.z);

		//すでに探索している情報であるならスキップ
		if (closed[currentIndex])
		{
			continue;
		}

		//通った経路を探索済みにする
		closed[currentIndex] = true;

		//ゴールに到達したら終了
		if (current.x == goalX && current.z == goalZ)
		{
			isFoundGoal = true;
			break;
		}

		//8方向探索
		for (int i = 0; i < 8; ++i)
		{
			int nx = current.x + kDirectionX[i];
			int nz = current.z + kDirectionZ[i];

			//隣のグリッドがステージ外ならスキップする
			const auto* nextGrid = pNaviGrid_->GetNode(nx, nz);
			if (!nextGrid || !nextGrid->iswalked)
			{
				continue;
			}

			int nextIndex = toIndex(nx, nz);
			//すでに探索を行っているならスキップ
			if (closed[nextIndex])
			{
				continue;
			}

			bool isDiagonal = (kDirectionX[i] != 0 && kDirectionZ[i] != 0);

			//斜め移動の際に探索でのすり抜けをチェックする
			if (isDiagonal)
			{
				const auto* sideA = pNaviGrid_->GetNode(current.x + kDirectionX[i], current.z);
				const auto* sideB = pNaviGrid_->GetNode(current.x, current.z + kDirectionZ[i]);

				//左右両方が壁の場合通れないのでスキップ
				if ((!sideA || !sideA->iswalked) && (!sideB || !sideB->iswalked))
				{
					continue;
				}
			}

			//次の移動にかかるコストを計算
			float moveCost = isDiagonal ? kDiagonalCost : kStraightCost;
			float newGCost = current.gCost + moveCost;

			//初めて訪れるグリッドか、今保存されているグリッドより小さいコストで進める場合
			if (bestGCost[nextIndex] < 0.0f || newGCost < bestGCost[nextIndex])
			{
				bestGCost[nextIndex] = newGCost;
				//どこから来たかを記録しておく
				parentX[nextIndex] = current.x;
				parentZ[nextIndex] = current.z;

				//新しい位置として登録
				AStarNode next;
				next.x = nx;
				next.z = nz;
				next.gCost = newGCost;
				next.hCost = Heuristic(nx, nz, goalX, goalZ);
				openList.push(next);
			}
		}
	}

	//ゴールが見つからない場合はからの配列を返す
	if (!isFoundGoal)
	{
		return resultPath;
	}

	//終点から逆にたどってスタートに戻る
	std::vector<Vector3> reversedPath;
	int traceX = goalX;
	int traceZ = goalZ;

	while (!(traceX == startX && traceZ == startZ))
	{
		const auto* node = pNaviGrid_->GetNode(traceX, traceZ);
		//3D空間上の座標を保持
		reversedPath.push_back(node->pos);

		int index = toIndex(traceX, traceZ);
		//1個前のグリッドに戻る
		int px = parentX[index];
		int pz = parentZ[index];
		traceX = px;
		traceZ = pz;
	}

	//始点の座標を追加
	const auto* startNodeForPath = pNaviGrid_->GetNode(startX, startZ);
	reversedPath.push_back(startNodeForPath->pos);

	//逆になっているのでスタート→ゴール順に反転させる
	resultPath.assign(reversedPath.rbegin(), reversedPath.rend());

	return resultPath;
}

float AStarPathFinder::Heuristic(int x1, int z1, int x2, int z2) const
{
	//斜め移動を考慮したユークリッド距離
	float dx = static_cast<float>(x2 - x1);
	float dz = static_cast<float>(z2 - z1);
	return std::sqrt(dx * dx + dz * dz);
}
