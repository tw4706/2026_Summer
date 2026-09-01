#pragma once
#include "Scene.h"
#include "Model.h"
#include "SceneManager.h"

class Bg;
class Model;
class TitlePlayer;
class TitleCamera;
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

	int currentEffectHandle_ = -1;//現在のエフェクトはンドル

	//タイトルロゴハンドル
	int titleLogoHandle_ = -1;

	//ゲーム開始のテキストハンドル
	int startTextHandle_ = -1;

	//ゲーム終了のテキストハンドル
	int endTextHandle_ = -1;

	//ゲーム開始の影テキストハンドル
	int startShadowTextHandle_ = -1;

	//ゲーム終了の影テキストハンドル
	int endShadowTextHandle_ = -1;

	//文字フレームのハンドル
	int textFrameHandle_ = -1;

	//タイトル用のステージハンドル
	Model stageModel_;

	//始めと終了の文字列のWidthとHeight
	int startTextWidth_ = 0, startTextHeight_ = 0;
	int endTextWidth_ = 0, endTextHeight_ = 0;
	int textFrameWidth_ = 0, textFrameHeight_ = 0;

	float startCurrentScale_ = 0.0f;

	float endCurrentScale_ = 0.0f;

	//背景
	std::shared_ptr<Bg>pBg_;

	//タイトル用プレイヤー
	std::shared_ptr<TitlePlayer>pTitlePlayer_;
};