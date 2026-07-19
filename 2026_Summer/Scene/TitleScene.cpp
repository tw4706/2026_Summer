#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "System/Input.h"
#include "Application.h"
#include "EffectManager.h"
#include<Dxlib.h>
#include<EffekseerForDXLib.h>
#include<memory>
#include<cassert>
#include<algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//タイトル画面の選択肢の描画時のオフセット
	constexpr float kTitleTextOffsetY = 100.0f;
	constexpr float kEndTextOffsetY = 130.0f;

	//ロゴの描画時のオフセット
	constexpr float kLogoOffsetX = -600.0f;
	constexpr float kLogoOffsetY = -500.0f;
	constexpr float kLogoScaleX = 0.6f;
	constexpr float kLogoScaleY = 0.6f;

	//選択肢の色
	constexpr unsigned int kColorSelected = 0xff0000;
	constexpr unsigned int kColorUnselected = 0x000000;
	constexpr unsigned int kColorBackground = 0xffffff;

	//シェーダーの登録番号
	constexpr int kDissolveShaderSlot = 4;

	//ディゾルブシェーダの強さ
	constexpr float kDissolveStrength = 0.05f;

	//選択肢の文字列
	const wchar_t* kTextStart = L"始める";
	const wchar_t* kTextEnd = L"ゲームを終了";

	//自作のシェーダを適用させた描画関数
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

	//エフェクトのロード
	EffectManager::GetInstance().Load(L"TitleCursole", "data/Effect/TitleCursole.efk");

	renderHandle_ = MakeScreen(Game::kScreenWidth, Game::kScreenHeight, true);

	//メモリの確保
	cBuffH_ = CreateShaderConstantBuffer(sizeof(ConstantBuffer));

	//CPU側にメモリを作る
	pCBuff_ = static_cast<ConstantBuffer*>(GetBufferShaderConstantBuffer(cBuffH_));

	frameCount_ = kFadeInterval;

	Vector3 firstEffectPos = {Game::kScreenWidth / 2.0f,Game::kScreenHeight / 2.0f + kTitleTextOffsetY,0.0f};
	currentEffectHandle_ = EffectManager::GetInstance().Play(L"TitleCursole", firstEffectPos);
	SetPosPlayingEffekseer3DEffect(currentEffectHandle_, firstEffectPos.x_, firstEffectPos.y_, firstEffectPos.z_);
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
	bool isChanged = false;

	if (Input::GetInstance().IsTriggered("up"))
	{
		currentIndex_ = 0;
		isChanged = true;
	}
	else if (Input::GetInstance().IsTriggered("down"))
	{
		currentIndex_ = 1;
		isChanged = true;
	}

	if (isChanged)
	{
		//エフェクトを停止
		EffectManager::GetInstance().Stop(currentEffectHandle_);

		//選択肢に応じたY軸のオフセットを計算
		float offsetY = (currentIndex_ == 0) ? kTitleTextOffsetY : kEndTextOffsetY;
		Vector3 nextPos = {Game::kScreenWidth / 2.0f,Game::kScreenHeight / 2.0f + offsetY,0.0f};

		//新しい位置で再生してハンドルを保存
		currentEffectHandle_ = EffectManager::GetInstance().Play(L"TitleCursole", nextPos);
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
	pCBuff_->strength = kDissolveStrength;
	pCBuff_->lightX = 0.0f;
	pCBuff_->lightY = 0.0f;

	//更新
	UpdateShaderConstantBuffer(cBuffH_);
	SetShaderConstantBuffer(cBuffH_, DX_SHADERTYPE_PIXEL, kDissolveShaderSlot);

	DrawGraphUseOrigShader(0, 0, renderHandle_, noiseHandle_, dissolvePSHandle_);
}

void TitleScene::NormalDraw()
{
	//DrawBox(0, 0, Game::kScreenWidth, Game::kScreenWidth, 0xffffff, true);

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

	int titleWidth = GetDrawStringWidth(kTextStart, 4);
	int endWidth = GetDrawStringWidth(kTextEnd, 6);

	DrawFormatString(Game::kScreenWidth / 2 - (titleWidth / 2), Game::kScreenHeight / 2 + kTitleTextOffsetY, titleColor, kTextStart);
	DrawFormatString(Game::kScreenWidth / 2 - (endWidth / 2), Game::kScreenHeight / 2 + kEndTextOffsetY, endColor, kTextEnd);

	//タイトルロゴの描画
	DrawRotaGraph3(Game::kScreenWidth / 2 + kLogoOffsetX, Game::kScreenHeight / 2 + kLogoOffsetY, 0, 0,
		kLogoScaleX, kLogoScaleY, 0.0f, titleLogoHandle_, true);
}