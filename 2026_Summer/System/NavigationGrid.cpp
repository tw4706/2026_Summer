#include "NavigationGrid.h"
#include <Dxlib.h>
#include <cmath>

namespace
{
	//レイを飛ばす始点の高さ
	const float kRayStartHeight = 1000.0f;

	//レイを飛ばす終点の高さ
	const float kRayEndHeight = -1000.0f;
}

void NavigationGrid::CreateGrid(int stageModelHandle, float minX, float maxX, float minZ, float maxZ, float cellSize)
{
	cellSize_ = cellSize;

	//グリッドの原点を記録する(座標を返還する際の基準点にするため)
	origin_ = Vector3{ minX, 0.0f, minZ };

	//マスの個数を求める
	width_ = static_cast<int>((maxX - minX) / cellSize_) + 1;
	height_ = static_cast<int>((maxZ - minZ) / cellSize_) + 1;

	//assignは値を明示的に指定できる
	//「既存の要素を消して新しく指定個数・指定値で埋め直す」関数
	nodes_.assign(width_ * height_, NodeData{});

	//各マスにRayを飛ばす
	for (int z = 0; z < height_; ++z)
	{
		for (int x = 0; x < width_; ++x)
		{
			Vector3 worldPos = GridToWorldPos(x, z);
			VECTOR start = VGet(worldPos.x_, kRayStartHeight, worldPos.z_);
			VECTOR end = VGet(worldPos.x_, kRayEndHeight, worldPos.z_);

			//ここで線分(Ray)とポリゴンの当たり判定を行うことで地面の高さがわかる
			MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageModelHandle, -1, start, end);

			NodeData& node = nodes_[z * width_ + x];

			//地面がない場合(ヒットしてないとき)
			if (hit.HitFlag == FALSE)
			{
				//歩行不可とする
				node.pos = Vector3{ worldPos.x_, 0.0f, worldPos.z_ };
				node.iswalked = false;
				continue;
			}

			//地面の高さの取得(ヒットしていた場合)
			float groundY = hit.HitPosition.y;
			node.pos = Vector3{ worldPos.x_, groundY, worldPos.z_ };

			//当たっているポリゴンの法線のY成分が閾値よりも高いかどうか判定
			//これにより「平坦な道かどうか」判定している
			bool isFlatEnough = hit.Normal.y >= kGroundNormalThreshold;
			bool isExpectedHeight = std::abs(groundY - expectedGroundY_) <= kGroundYTolerance;

			node.iswalked = isFlatEnough && isExpectedHeight;
		}
	}
}

const NavigationGrid::NodeData* NavigationGrid::GetNode(int x, int z) const
{
	if (x < 0 || x >= width_ || z < 0 || z >= height_)
	{
		return nullptr;
	}
	//インデックスにアクセスするための計算
	return &nodes_[z * width_ + x];
}

Vector3 NavigationGrid::GridToWorldPos(int x, int z) const
{
	return Vector3{ origin_.x_ + x * cellSize_, 0.0f, origin_.z_ + z * cellSize_ };
}

void NavigationGrid::WorldPosToGrid(const Vector3& worldPos, int& outX, int& outZ) const
{
	//今一番近いノードに変換する
	outX = static_cast<int>((worldPos.x_ - origin_.x_) / cellSize_ + 0.5f);
	outZ = static_cast<int>((worldPos.z_ - origin_.z_) / cellSize_ + 0.5f);
}
