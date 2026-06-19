#include "Model.h"
#include<Dxlib.h>
#include<cassert>

Model::Model() :
	modelH_(-1)
{
}

Model::~Model()
{
	Release();
}

void Model::Load(const char* path)
{
	//ゴミがある可能性があるため念のため解放しておく
	Release();

	//モデルのロード
	modelH_ = MV1LoadModel(path);
	assert(modelH_ >= 0);
}

void Model::Release()
{
	if (modelH_ != -1)
	{
		//モデルの解放
		MV1DeleteModel(modelH_);
		modelH_ = -1;//ハンドルも-1にしておく
	}
}

void Model::Draw()
{
	if (modelH_ != -1)
	{
		//モデルの描画
		MV1DrawModel(modelH_);
	}
}

void Model::SetMatrix(const Matrix4x4& mat)
{
	//モデルに行列を適用させる
	MV1SetMatrix(modelH_, mat.ToDxLibMatrix());
}

void Model::SetMatrix(const MATRIX& mat)
{
	//モデルに行列を適用させる
	MV1SetMatrix(modelH_, mat);
}

int Model::SearchFrame(const char* frameName) const
{
	return MV1SearchFrame(modelH_, frameName);
}
