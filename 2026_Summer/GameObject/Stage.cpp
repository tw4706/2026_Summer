#include "Stage.h"
#include "Collider/PolygonCollider.h"

namespace
{
	//ナビゲーショングリッドを生成する範囲
	const float kGridMinX = -500.0f;
	const float kGridMaxX = 500.0f;
	const float kGridMinZ = -500.0f;
	const float kGridMaxZ = 500.0f;
	//グリッド1マスのサイズ
	const float kGridCellSize = 50.0f;

	//バウンディングボックスを作成する際の余白
	const float kBoundsMargin = 50.0f;
}

Stage::Stage(Vector3 pos, Vector3 vel, float dir) :
	Collidable(pos, vel, dir)
{
}

Stage::~Stage()
{
}

void Stage::Init()
{
	//モデルのロード
	stageModel_.Load(L"data/MV1/Stage.mv1");

	MV1SetRotationXYZ(stageModel_.GetHandle(), VGet(0.0f, -DX_PI_F / 2.0f, 0.0f));
	MV1SetPosition(stageModel_.GetHandle(), VGet(0.0f, -100.0f, 0.0f));

	//モデルの総ポリゴン当たり判定データを構築
	MV1SetupCollInfo(stageModel_.GetHandle(), -1);
	MV1RefreshCollInfo(stageModel_.GetHandle(), -1);

	//コライダーの生成
	this->CreateCollider<PolygonCollider>(stageModel_.GetHandle());

	//ナビゲーショングリッドの生成
	navGrid_.SetExpectedGroundY(-100.0f);
	navGrid_.CreateGrid(stageModel_.GetHandle(), kGridMinX, kGridMaxX, kGridMinZ, kGridMaxZ, kGridCellSize);

}

void Stage::Update()
{

}

void Stage::Draw()
{
	//モデルの描画
	stageModel_.Draw();

#ifdef _DEBUG
	//ナビゲーショングリッドデバッグの表示
	//DrawNavGridDebug();
#endif
}

void Stage::DrawNavGridDebug() const
{
	for (int z = 0; z < navGrid_.GetHeight(); ++z)
	{
		for (int x = 0; x < navGrid_.GetWidth(); ++x)
		{
			const NavigationGrid::NodeData* node = navGrid_.GetNode(x, z);
			if (!node) continue;

			//地面に埋まって見えなくなるのを防ぐため少し浮かせる
			Vector3 pos = VGet(node->pos.x_, node->pos.y_ + 5.0f, node->pos.z_);

			//歩行可能なら緑、不可能なら赤
			unsigned int color = node->iswalked ? GetColor(0, 255, 0) : GetColor(255, 0, 0);

			DrawSphere3D(pos.ToDxlibVector(), 5.0f, 6, color, color, TRUE);
		}
	}
}