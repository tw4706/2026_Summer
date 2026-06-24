#pragma once
#include "GameObject.h"
#include "Model.h"

class Stage :public GameObject
{
public:
	Stage(Vector3 pos, Vector3 vel, float dir);
	~Stage();

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// ハンドルの取得
	/// </summary>
	/// <returns>ハンドルを返す</returns>
	int GetHandle()const { return stageModel_.GetHandle(); }
private:
	Model stageModel_;
};

