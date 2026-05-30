#pragma once
#include<memory>

class Player;
class Camera;
class Input;
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

	std::shared_ptr<Player>pPlayer_;//プレイヤー
	std::shared_ptr<Camera>pCamera_;//カメラ
};

