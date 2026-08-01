#pragma once
#include "Scene.h"
#include "SceneManager.h"

class TitleScene :public Scene
{
public:
	TitleScene(SceneManager& sceneManager);
	~TitleScene();

	void Init()override;
	void Update();
	void Draw();

	void FadeInUpdate();
	void NormalUpdate();
	void FadeOutUpdate();
	using UpdateFunc_t = void (TitleScene::*)();
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (TitleScene::*)();
	DrawFunc_t draw_;

private:

	int frameCount_ = 0;

	int currentIndex_ = 0;	//現在選ばれている要素

	int currentEffectHandle_=-1;//現在のエフェクトはンドル

	//タイトルロゴハンドル
	int titleLogoHandle_ = -1;
};

