#include "Stage.h"
#include "Collider/PolygonCollider.h"

namespace
{
	//ナビゲーショングリッドを生成する範囲
	constexpr float kGridMinX = -5000.0f;
	constexpr float kGridMaxX = 1000.0f;
	constexpr float kGridMinZ = -8500.0f;
	constexpr float kGridMaxZ = 0.0f;

	//グリッド1マスのサイズ
	constexpr float kGridCellSize = 120.0f;

	//バウンディングボックスを作成する際の余白
	constexpr int kBoundMargin = 180;

	//ステージのY軸回転
	constexpr float kStageRotateY = -DX_PI_F / 2.0f;

	//ステージの初期位置
	const Vector3 kFirstStageColPos = { 0.0f, -100.0f, 0.0f };
	const Vector3 kFirstStageModelPos = { 0.0f, -70.0f, 0.0f };

	//地面の高さの限界
	constexpr float kGroundY = -100.0f;

	//デバッグノードのオフセット
	constexpr float kDebugNodeOffset = -5.0f;

	//デバッグ描画用のグリッドの半径
	constexpr float kDebugGridRadius = 5.0f;

	//デバッグ描画用のDivNum
	constexpr int kDebugGridDiv = 6;
}

Stage::Stage():
	Collidable(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, 0.0f)
{
}

Stage::Stage(Vector3 pos, Vector3 vel, float dir) :
	Collidable(pos, vel, dir)
{
}

Stage::~Stage()
{
	//モデルの削除
	stageColModel_.Release();
}

void Stage::Init()
{
	//モデルのロード
	stageColModel_.Load(L"data/MV1/StageCol.mv1");
	stageModel_.Load(L"data/MV1/StageModel.mv1");


	//ステージの当たり判定のモデルと実際に描画しているモデルの回転とポジションの設定
	MV1SetRotationXYZ(stageColModel_.GetHandle(), VGet(0.0f, kStageRotateY, 0.0f));
	MV1SetPosition(stageColModel_.GetHandle(), kFirstStageColPos.ToDxlibVector());

	MV1SetRotationXYZ(stageModel_.GetHandle(), VGet(0.0f, kStageRotateY, 0.0f));
	MV1SetPosition(stageModel_.GetHandle(), kFirstStageModelPos.ToDxlibVector());

	//モデルの総ポリゴン当たり判定データを構築
	MV1SetupCollInfo(stageColModel_.GetHandle(), -1);
	MV1RefreshCollInfo(stageColModel_.GetHandle(), -1);

	//コライダーの生成
	this->CreateCollider<PolygonCollider>(stageColModel_.GetHandle());

	//ナビゲーショングリッドの生成
	navGrid_.SetExpectedGroundY(kGroundY);
	navGrid_.CreateGrid(stageColModel_.GetHandle(), kGridMinX, kGridMaxX, kGridMinZ, kGridMaxZ, kGridCellSize, kBoundMargin);

}

void Stage::Update()
{
	//今のところステージはおいているだけ
}

void Stage::Draw()
{
	//モデルの描画
	stageModel_.Draw();

#ifdef _DEBUG
	//ナビゲーショングリッドデバッグの表示
	DrawNavGridDebug();
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
			Vector3 pos = node->pos;
			pos.y_ += kDebugNodeOffset;

			//歩行可能なら緑不可能なら赤
			unsigned int color = node->iswalked ? GetColor(0, 255, 0) : GetColor(0, 255, 255);

			DrawSphere3D(pos.ToDxlibVector(), kDebugGridRadius, kDebugGridDiv, color, color, true);
		}
	}
}