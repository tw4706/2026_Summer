#include "Stage.h"

Stage::Stage(Vector3 pos,Vector3 vel,float dir):
	GameObject(pos,vel,dir)
{
}

Stage::~Stage()
{
}

void Stage::Init()
{
	//モデルのロード
	stageModel_.Load(L"data/Stage.mv1");

	MV1SetPosition(stageModel_.GetHandle(), VGet(0.0f, 0.0f, 0.0f));

	//モデルの総ポリゴン当たり判定データを構築
	MV1SetupCollInfo(stageModel_.GetHandle(), -1);
}

void Stage::Update()
{

}

void Stage::Draw()
{
	//モデルの描画
	stageModel_.Draw();
}