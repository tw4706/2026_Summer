#pragma once
#include"Math/Vector3.h"
#include<vector>
class NavigationGrid
{
public:
	//グリッド1つごとのデータ
	struct NodeData
	{
		Vector3 pos;//座標
		bool iswalked = true;//歩行できるかどうか
	};

	/// <summary>
	/// 指定された範囲とセルサイズでグリッドを作成
	/// </summary>
	/// <param name="stageModelHandle">グリッドを作成するステージモデルハンドル</param>
	/// <param name="minX">グリッドのX軸方向の最小座標</param>
	/// <param name="maxX">グリッドのX軸方向の最大座標</param>
	/// <param name="minZ">グリッドのZ軸方向の最小座標</param>
	/// <param name="maxZ">グリッドのZ軸方向の最大座標</param>
	/// <param name="cellSize">各グリッドの1マスのサイズ</param>
	void CreateGrid(int stageModelHandle, float minX, float maxX, float minZ, float maxZ, float cellSize);

	/// <summary>
	/// x,z座標に対応するノードデータの座標を取得
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="z">Z座標</param>
	/// <returns></returns>
	const NodeData* GetNode(int x, int z)const;

	/// <summary>
	/// グリッドの座標をワールド座標に変換
	/// </summary>
	/// <param name="x">グリッド空間でのX座標</param>
	/// <param name="z">グリッド空間での座標</param>
	/// <returns>変換後のワールド座標</returns>
	Vector3 GridToWorldPos(int x, int z)const;

	/// <summary>
	/// ワールド座標をグリッド座標に変換します。
	/// </summary>
	/// <param name="worldPos">変換するワールド座標の参照</param>
	/// <param name="outX">変換後のグリッド X 座標の参照</param>
	/// <param name="outZ">変換後のグリッド Z 座標の参照</param>
	void WorldPosToGrid(const Vector3& worldPos, int& outX, int& outZ) const;

	//ゲッター
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	float GetCellSize() const { return cellSize_; }

	/// <summary>
	/// 想定する地面の高さを設定する
	/// </summary>
	/// <param name="y">地面の高さ</param>
	void SetExpectedGroundY(float y) { expectedGroundY_ = y; }

private:
	std::vector<NodeData> nodes_; //グリッド全体のノード配列
	int width_ = 0;
	int height_ = 0;
	float cellSize_ = 50.0f;
	Vector3 gridPos_; //グリッドの原点

	//想定する地面の高さ
	float expectedGroundY_ = 0.0f;
};

