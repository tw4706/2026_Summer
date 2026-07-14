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
	//ディゾルブ用の定数バッファ
	struct DissolveBufferData
	{
		float dissolve[4];
	};

	int frameCount_ = 0;

	int currentIndex_ = 0;	//現在選ばれている要素

	//タイトルロゴハンドル
	int titleLogoHandle_ = -1;
	int noiseHandle_ = -1;				//ノイズテクスチャハンドル
	int dissolvePSHandle_ = -1;			//ディゾルブピクセルシェーダ用のハンドル
	int dissolveConstBufferHandle_ = -1;//コンスタントバッファハンドル
	int renderHandle_ = -1;				//レンダリング用はンドル
};

