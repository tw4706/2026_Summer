#pragma once
#include "Scene.h"
#include "SceneManager.h"

class Bg;
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

	//ゲーム開始のテキストハンドル
	int startTextHandle_ = -1;

	//ゲーム終了のテキストハンドル
	int endTextHandle_ = -1;

	int startTextWidth_ = 0, startTextHeight_ = 0;
	int endTextWidth_ = 0, endTextHeight_ = 0;

	//背景
	std::shared_ptr<Bg>pBg_;
};