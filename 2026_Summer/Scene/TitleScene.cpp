#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "System/Input.h"
#include "Application.h"
#include<Dxlib.h>
#include<memory>
#include<algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;
}

TitleScene::TitleScene(SceneManager& sceneManager) :
	Scene(sceneManager),
	update_(&TitleScene::FadeInUpdate),
	draw_(&TitleScene::FadeDraw),
	frameCount_(kFadeInterval)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	titleLogoHandle_ = LoadGraph(L"data/UI/titleLogo.png");

	//ディゾルブ用ノイズ画像のロード
	noiseHandle_ = LoadGraph(L"data/Shader/noise.png");
	//ピクセルシェーダのロード
	dissolvePSHandle_ = LoadPixelShader(L"../DissolvePS.pso");
	dissolveConstBufferHandle_ = CreateShaderConstantBuffer(sizeof(DissolveBufferData));

	//画面全体を一旦描き込むためのレンダーターゲット
	renderHandle_ = MakeScreen(Game::kScreenWidth, Game::kScreenHeight, true);
}

void TitleScene::Update()
{
	(this->*update_)();
}

void TitleScene::Draw()
{
	(this->*draw_)();
}

void TitleScene::FadeInUpdate()
{
	if (frameCount_-- <= 0)
	{
		update_ = &TitleScene::NormalUpdate;
		draw_ = &TitleScene::NormalDraw;
	}
}

void TitleScene::NormalUpdate()
{
	if (Input::GetInstance().IsTriggered("up"))
	{
		currentIndex_ = 0;
	}
	else if (Input::GetInstance().IsTriggered("down"))
	{
		currentIndex_ = 1;
	}

	if (Input::GetInstance().IsTriggered("next"))
	{
		if (currentIndex_ == 0)
		{
			frameCount_ = kFadeInterval;

			update_ = &TitleScene::FadeOutUpdate;
			draw_ = &TitleScene::FadeDraw;
		}
		else if (currentIndex_ == 1)
		{
			Application::GetInstance().GameEnd();
		}

	}
}

void TitleScene::FadeOutUpdate()
{
	if (frameCount_-- <= 0)
	{
		sceneManager_.ChangeScene(std::make_shared<GameScene>(sceneManager_));
	}
}

void TitleScene::FadeDraw()
{
	float rate;

	if (update_ == &TitleScene::FadeInUpdate)
	{
		// フェードイン
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//フェードアウト
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}
	rate = std::clamp(rate, 0.0f, 1.0f);

	// ① 通常描画分をオフスクリーンに描く
	int prevScreen = GetDrawScreen();
	SetDrawScreen(renderHandle_);
	ClearDrawScreen();
	NormalDraw();
	SetDrawScreen(prevScreen);

	// ② 定数バッファを更新
	DissolveBufferData* bufferData =
		(DissolveBufferData*)GetBufferShaderConstantBuffer(dissolveConstBufferHandle_);
	bufferData->dissolve[0] = rate;		//進行度
	bufferData->dissolve[1] = 0.08f;	//境界のぼかし幅
	bufferData->dissolve[2] = 0.0f;
	bufferData->dissolve[3] = 0.0f;
	UpdateShaderConstantBuffer(dissolveConstBufferHandle_);
	SetShaderConstantBuffer(dissolveConstBufferHandle_, DX_SHADERTYPE_PIXEL, 0);

	//ノイズテクスチャをスロット1にセット
	SetUseTextureToShader(1, noiseHandle_);

	//ピクセルシェーダーをレンダリング用スクリーンに描画
	SetUsePixelShader(dissolvePSHandle_);
	DrawGraph(0, 0, renderHandle_, TRUE);
	SetUsePixelShader(-1);

	//シェーダの削除
	SetUseTextureToShader(1, -1);
}

void TitleScene::NormalDraw()
{
	int titleColor = 0;
	int endColor = 0;

	if (currentIndex_ == 0)
	{
		titleColor = 0xff0000;
		endColor = 0xffffff;
	}
	else if (currentIndex_ == 1)
	{
		titleColor = 0xffffff;
		endColor = 0xff0000;
	}

	int titleWidth = GetDrawStringWidth(L"始める", 4);
	int endWidth = GetDrawStringWidth(L"ゲームを終了", 6);

	DrawFormatString(Game::kScreenWidth / 2.0f - (titleWidth / 2), Game::kScreenHeight / 2.0f+100, titleColor, L"始める");
	DrawFormatString(Game::kScreenWidth / 2.0f - (endWidth/2), Game::kScreenHeight / 2.0f + 130, endColor, L"ゲームを終了");

	//タイトルロゴの描画
	DrawRotaGraph3(Game::kScreenWidth / 2 - 600, Game::kScreenHeight / 2 - 500, 0, 0,
		0.6f, 0.6f, 0.0f, titleLogoHandle_, true);
}
