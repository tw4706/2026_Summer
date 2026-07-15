#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "System/Input.h"
#include "Application.h"
#include<Dxlib.h>
#include<memory>
#include<cassert>
#include<algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;


	void DrawGraphUseOrigShader(const int x, const int y, const int texH, const int psH, const int psShaderH)
	{
		// 板ポリゴンを構成するための4つの頂点を宣言
		const int kVertNum = 4;
		std::array<VERTEX2DSHADER, 4> vertices{};

		for (auto& vertex : vertices)
		{
			vertex.rhw = 1.0f;
			vertex.dif = GetColorU8(255, 255, 255, 255);
			vertex.spc = GetColorU8(0, 0, 0, 0);
		}

		int graphWidth, graphHeight;
		GetGraphSize(texH, &graphWidth, &graphHeight);
		float rectStartX = x;
		float rectStartY = y;
		float rectEndX = x + graphWidth;
		float rectEndY = y + graphHeight;

		//座標
		vertices[0].pos = { rectStartX, rectStartY, 0 };
		vertices[0].u = 0.0f; vertices[0].v = 0.0f;
		vertices[0].su = 0.0f; vertices[0].sv = 0.0f;

		vertices[1].pos = { rectEndX, rectStartY, 0 };
		vertices[1].u = 1.0f; vertices[1].v = 0.0f;
		vertices[1].su = 1.0f; vertices[1].sv = 0.0f;

		vertices[2].pos = { rectStartX, rectEndY, 0 };
		vertices[2].u = 0.0f; vertices[2].v = 1.0f;
		vertices[2].su = 0.0f; vertices[2].sv = 1.0f;

		vertices[3].pos = { rectEndX, rectEndY, 0 };
		vertices[3].u = 1.0f; vertices[3].v = 1.0f;
		vertices[3].su = 1.0f; vertices[3].sv = 1.0f;

		//インデックスデータ
		unsigned short index[6] = { 0, 1, 2, 2, 1, 3 };

		//シェーダーの適用
		SetUsePixelShader(psShaderH);

		//テクスチャの適用
		SetUseTextureToShader(0, texH);     //register(t0)に描画先スクリーン
		SetUseTextureToShader(1, psH);		//register(t1)にノイズ画像

		//ポリゴン数は三角形の二つ
		const int kPolyNum = 2;
		DrawPolygonIndexed2DToShader(vertices.data(), static_cast<int>(vertices.size()), index, kPolyNum);
	}
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
	DeleteShaderConstantBuffer(cBuffH_);
}

void TitleScene::Init()
{
	titleLogoHandle_ = LoadGraph(L"data/UI/titleLogo.png");

	//ディゾルブ用ノイズ画像のロード
	noiseHandle_ = LoadGraph(L"data/Shader/noise.png");
	assert(noiseHandle_ >= 0);

	//ピクセルシェーダのロード
	dissolvePSHandle_ = LoadPixelShader(L"DissolvePS.pso");
	assert(dissolvePSHandle_ >= 0);

	renderHandle_ = MakeScreen(Game::kScreenWidth, Game::kScreenHeight, true);

	//メモリの確保
	cBuffH_ = CreateShaderConstantBuffer(sizeof(ConstantBuffer));

	//CPU側にメモリを作る
	pCBuff_ = static_cast<ConstantBuffer*>(GetBufferShaderConstantBuffer(cBuffH_));

	frameCount_ = kFadeInterval;
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
	frameCount_--;

	if (frameCount_ <= 0)
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
	frameCount_--;

	if (frameCount_ <= 0)
	{
		sceneManager_.ChangeScene(std::make_shared<GameScene>(sceneManager_));
	}
}

void TitleScene::FadeDraw()
{
	float rate;

	if (update_ == &TitleScene::FadeInUpdate)
	{
		//フェードイン
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//フェードアウト
		rate = (float)frameCount_ / kFadeInterval;
	}
	rate = std::clamp(rate, 0.0f, 1.0f);

	int prevScreen = GetDrawScreen();
	SetDrawScreen(renderHandle_);
	ClearDrawScreen();
	NormalDraw();
	SetDrawScreen(prevScreen);

	//メモリの値を変更
	pCBuff_->value = rate;
	pCBuff_->strength = 0.05f;
	pCBuff_->lightX = 0.0f;
	pCBuff_->lightY = 0.0f;

	//更新
	UpdateShaderConstantBuffer(cBuffH_);
	SetShaderConstantBuffer(cBuffH_, DX_SHADERTYPE_PIXEL, 4);

	DrawGraphUseOrigShader(0, 0, renderHandle_, noiseHandle_,dissolvePSHandle_);
}

void TitleScene::NormalDraw()
{
	DrawBox(0,0,Game::kScreenWidth, Game::kScreenWidth, 0xffffff,true);

	int titleColor = 0;
	int endColor = 0;

	if (currentIndex_ == 0)
	{
		titleColor = 0xff0000;
		endColor = 0x000000;
	}
	else if (currentIndex_ == 1)
	{
		titleColor = 0x000000;
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