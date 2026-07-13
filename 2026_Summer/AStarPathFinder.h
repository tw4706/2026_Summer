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

	/// <summary>
	/// ナビゲーショングリッドのセット
	/// </summary>
	/// <param name="pNavGrid">ナビゲーショングリッド</param>
	void SetNavigationGrid(const NavigationGrid* pNavGrid);

	/// <summary>
	/// パスの探索
	/// </summary>
	/// <param name="startPos">始点座標</param>
	/// <param name="endPos">終点座標</param>
	/// <returns>計算した結果の経路を返す</returns>
	std::vector<Vector3>FindPath(const Vector3& startPos, const Vector3& endPos)const;
private:

	struct AStarNode
	{
		//グリッド上での2次元座標に使うx,z座標
		int x = 0;
		int z = 0;

		//実コスト
		//始点からゴールまでのコスト
		float gCost = 0.0f;

		//予測コスト
		//なにも考慮していない純粋な直線上での距離
		float hCost = 0.0f;

		//上記の実コストと予測コストを合計を出す関数
		//A*探索を用いた経路探索の場合は一番小さいマスを進む
		float fCost()const { return gCost + hCost; }
	};

	//グリッドのマス目から終点までの2点間の直線距離を計算
	//ユークリッド距離で計算(中身はsqrtで計算している)
	//この計算を行うことで無駄に計算せず最短距離で敵が移動てくれる
	float Heuristic(int x1, int z1, int x2, int z2)const;
private:

	//グリッドのポインタ
	const NavigationGrid* pNaviGrid_ = nullptr;
};

