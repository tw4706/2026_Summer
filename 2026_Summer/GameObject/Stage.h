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
private:
	Model stageModel_;
};

