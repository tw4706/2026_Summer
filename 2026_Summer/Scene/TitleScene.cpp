#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include"Bg.h"
#include "System/Input.h"
#include "Application.h"
#include "EffectManager.h"
#include "FadeManager.h"
#include "SoundManager.h"
#include "TitlePlayer.h"
#include<Dxlib.h>
#include<memory>
#include<cassert>
#include<algorithm>
#include<EffekseerForDXLib.h>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//タイトル画面の選択肢の描画時のオフセット
	constexpr int kTitleTextOffsetY = 60;
	constexpr int kEndTextOffsetY = 160;

	//ロゴの描画時のオフセット
	constexpr int kLogoOffsetX = -600;
	constexpr int kLogoOffsetY = -500;
	constexpr float kLogoScaleX = 0.6f;
	constexpr float kLogoScaleY = 0.6f;

	//選択肢の色
	constexpr unsigned int kColorSelected = 0xff0000;
	constexpr unsigned int kColorUnselected = 0x000000;

	//シェーダーの登録番号
	constexpr int kDissolveShaderSlot = 4;

	//ディゾルブシェーダの強さ
	constexpr float kDissolveStrength = 0.05f;

	//選択されているときの拡大率
	constexpr float kSelectedScale = 0.8f;
	constexpr float kUnselectedScale = 0.6f;

	//選択肢の文字の拡大率の補間割合
	constexpr float kScaleLerpRate = 0.1f;
}

TitleScene::TitleScene(SceneManager& sceneManager) :
	Scene(sceneManager),
	update_(&TitleScene::FadeInUpdate),
	draw_(&TitleScene::FadeDraw),
	frameCount_(kFadeInterval)
{
	pBg_ = std::make_shared<Bg>();
	pTitlePlayer_ = std::make_shared<TitlePlayer>();
}

TitleScene::~TitleScene()
{
	//ハンドルの削除
	DeleteGraph(titleLogoHandle_);
	DeleteGraph(startTextHandle_);
	DeleteGraph(endTextHandle_);
}

void TitleScene::Init()
{
	frameCount_ = kFadeInterval;

	//ハンドルの読み込み
	titleLogoHandle_ = LoadGraph(L"data/UI/titleLogo.png");
	startTextHandle_ = LoadGraph(L"data/UI/start.png");
	endTextHandle_ = LoadGraph(L"data/UI/endText.png");

	//ハンドルサイズの取得
	GetGraphSize(startTextHandle_, &startTextWidth_, &startTextHeight_);
	GetGraphSize(endTextHandle_, &endTextWidth_, &endTextHeight_);

	//背景の初期化
	pBg_->Init((L"data/Bg/backGround"));

	//タイトル用プレイヤーの初期化とアニメーション設定
	pTitlePlayer_->Init();

	SoundManager::GetInstance().PlayBgm(BGM::Title);
}

void TitleScene::Update()
{
	//タイトル用プレイヤーの更新
	pTitlePlayer_->Update();

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
		SoundManager::GetInstance().PlaySe(SE::CursoleMove);
	}
	else if (Input::GetInstance().IsTriggered("down"))
	{
		currentIndex_ = 1;
		SoundManager::GetInstance().PlaySe(SE::CursoleMove);
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
		SoundManager::GetInstance().PlaySe(SE::Decide);
	}

	//背景の更新
	pBg_->Update();
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

	FadeManager::GetInstance().StartCapture();
	NormalDraw();
	FadeManager::GetInstance().EndCaptureAndDraw(rate);
}

void TitleScene::NormalDraw()
{
	pBg_->Draw(Vector3{ 0.0f,0.0f,0.0f });

	pTitlePlayer_->Draw();

	float startScale = (currentIndex_ == 0) ? kSelectedScale : kUnselectedScale;
	float endScale = (currentIndex_ == 1) ? kSelectedScale : kUnselectedScale;

	startCurrentScale_ = Vector3::Lerp(startCurrentScale_, startScale, kScaleLerpRate);
	endCurrentScale_ = Vector3::Lerp(endCurrentScale_, endScale, kScaleLerpRate);

	DrawRotaGraph3(Game::kScreenWidth / 2, Game::kScreenHeight / 2 + kTitleTextOffsetY,
		startTextWidth_ / 2, startTextHeight_ / 2, startCurrentScale_, startCurrentScale_, 0.0f, startTextHandle_, true);

	DrawRotaGraph3(Game::kScreenWidth / 2, Game::kScreenHeight / 2 + kEndTextOffsetY,
		endTextWidth_ / 2, endTextHeight_ / 2, endCurrentScale_, endCurrentScale_, 0.0f, endTextHandle_, true);

	//タイトルロゴの描画
	DrawRotaGraph3(Game::kScreenWidth / 2 + kLogoOffsetX, Game::kScreenHeight / 2 + kLogoOffsetY, 0, 0,
		kLogoScaleX, kLogoScaleY, 0.0f, titleLogoHandle_, true);
}