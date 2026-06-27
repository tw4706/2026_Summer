#include "Stage.h"
#include "Collider/PolygonCollider.h"

Stage::Stage(Vector3 pos,Vector3 vel,float dir):
	Collidable(pos,vel,dir)
{
}

Stage::~Stage()
{
}

void Stage::Init()
{
	//モデルのロード
	stageModel_.Load(L"data/Stage.mv1");

	MV1SetRotationXYZ(stageModel_.GetHandle(), VGet(0.0f, -DX_PI_F / 2.0f, 0.0f));
	MV1SetPosition(stageModel_.GetHandle(), VGet(0.0f, -100.0f, 0.0f));

	//モデルの総ポリゴン当たり判定データを構築
	MV1SetupCollInfo(stageModel_.GetHandle(), -1);
	MV1RefreshCollInfo(stageModel_.GetHandle(), -1);

	//コライダーの生成
	this->CreateCollider<PolygonCollider>(stageModel_.GetHandle());
}

void Stage::Update()
{

}

void Stage::Draw()
{
	//モデルの描画
	stageModel_.Draw();
}