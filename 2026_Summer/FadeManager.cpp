#include "FadeManager.h"
#include "Game.h"
#include <Dxlib.h>
#include <array>

namespace
{
	//シェーダーの登録番号
	constexpr int kDissolveShaderSlot = 4;
	//ディゾルブシェーダの強さ
	constexpr float kDissolveStrength = 0.05f;

	//自作のシェーダを適用させた描画関数
	void DrawGraphUseOrigShader(const int x, const int y, const int texH, const int psH, const int psShaderH)
	{
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
		float rectStartX = static_cast<float>(x);
		float rectStartY = static_cast<float>(y);
		float rectEndX = static_cast<float>(x + graphWidth);
		float rectEndY = static_cast<float>(y + graphHeight);

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

		unsigned short index[6] = { 0, 1, 2, 2, 1, 3 };

		SetUsePixelShader(psShaderH);
		SetUseTextureToShader(0, texH);
		SetUseTextureToShader(1, psH);

		const int kPolyNum = 2;
		DrawPolygonIndexed2DToShader(vertices.data(), static_cast<int>(vertices.size()), index, kPolyNum);
	}
}

FadeManager& FadeManager::GetInstance()
{
	static FadeManager instance;
	return instance;
}

void FadeManager::Init()
{
	//既に初期化済みなら何もしない
	if (isInitialized_)return;

	noiseHandle_ = LoadGraph(L"data/Shader/noise.png");
	dissolvePSHandle_ = LoadPixelShader(L"DissolvePS.pso");
	renderHandle_ = MakeScreen(Game::kScreenWidth, Game::kScreenHeight, true);

	cBuffH_ = CreateShaderConstantBuffer(sizeof(ConstantBuffer));
	pCBuff_ = static_cast<ConstantBuffer*>(GetBufferShaderConstantBuffer(cBuffH_));

	isInitialized_ = true;
}

void FadeManager::Release()
{
	if (!isInitialized_)return;

	DeleteGraph(noiseHandle_);
	DeleteGraph(renderHandle_);
	DeleteGraph(dissolvePSHandle_);
	DeleteShaderConstantBuffer(cBuffH_);

	isInitialized_ = false;
}

void FadeManager::StartCapture()
{
	//現在の描画先を保存してからキャプチャ用の描画先に切り替える
	prevScreen_ = GetDrawScreen();
	SetDrawScreen(renderHandle_);
	ClearDrawScreen();
}

void FadeManager::EndCaptureAndDraw(float rate)
{
	//描画先を元に戻す
	SetDrawScreen(prevScreen_);

	//定数バッファの更新
	pCBuff_->value = rate;
	pCBuff_->strength = kDissolveStrength;
	pCBuff_->lightX = 0.0f;
	pCBuff_->lightY = 0.0f;

	UpdateShaderConstantBuffer(cBuffH_);
	SetShaderConstantBuffer(cBuffH_, DX_SHADERTYPE_PIXEL, kDissolveShaderSlot);

	DrawGraphUseOrigShader(0, 0, renderHandle_, noiseHandle_, dissolvePSHandle_);
}
