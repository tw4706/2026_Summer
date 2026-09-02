#pragma once
#include "Scene.h"
#include "UI/ButtonUI.h"
class PauseScene :public Scene
{
public:
	PauseScene(SceneManager& sceneManager);
	~PauseScene();

	void Init()override;
	void Update();
	void Draw();

	void FadeInUpdate();
	void NormalUpdate();
	void FadeOutUpdate();
	using UpdateFunc_t = void (PauseScene::*)();
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (PauseScene::*)();
	DrawFunc_t draw_;

private:
	int frameCount_ = 0;
	int currentIndex_ = 0;
	bool isInputEnabled_ = false;

	//X,Y,Aボタン
	ButtonUI buttonX_;
	ButtonUI buttonY_;
	ButtonUI buttonA_;

	//L,Rボタン
	ButtonUI buttonL_;
	ButtonUI buttonR_;
};

