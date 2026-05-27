#pragma once
#include"Player.h"
#include<memory>

class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();

private:
	void DrawGrid();

private:
	int m_frameCount;

	std::shared_ptr<Player>pPlayer_;
};

