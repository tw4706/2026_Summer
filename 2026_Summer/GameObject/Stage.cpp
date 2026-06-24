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
	//ステージのロード
	stageModel_.Load(L"data/Stage.mv1");
}

void Stage::Update()
{

}

void Stage::Draw()
{
	//モデルの描画
	stageModel_.Draw();
}