#pragma once
#include "Scene.h"

class ResultScene :public Scene
{
public:
	ResultScene(SceneManager& sceneManager,float clearTime);
	~ResultScene();

	void Init()override;
	void Update();
	void Draw();

	void FadeInUpdate();
	void NormalUpdate();
	void FadeOutUpdate();
	using UpdateFunc_t = void (ResultScene::*)();
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (ResultScene::*)();
	DrawFunc_t draw_;

private:
	int frameCount_ = 0;

	float clearTime_;
	int currentIndex_ = 0;
	wchar_t rank_;
};

