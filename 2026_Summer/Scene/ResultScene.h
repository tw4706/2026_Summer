#pragma once
#include "Scene.h"
#include "Bg.h"
#include<memory>

class ResultScene :public Scene
{
public:
	ResultScene(SceneManager& sceneManager, float clearTime, bool isGameOver);
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
	int performanceCount_ = 0;			//演出用のカウント
	int rankHandle_ = -1;				//ランクの画像ハンドル

	float displayedTime_ = 0.0f;		//カウントアップ中の表示タイム
	float clearTime_;					//クリアタイム
	int currentIndex_ = 0;				//0の場合はリトライで1の場合はタイトルに戻る
	wchar_t rank_ = 0;					//表示ランク
	bool isInputEnabled_ = false;		//入力を受け付けるフラグ
	bool isGameOver_ = false;			//ゲームオーバーかどうかのフラグ

	std::shared_ptr<Bg> pBg_;			//背景
};

