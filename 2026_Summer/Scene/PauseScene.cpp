#include "PauseScene.h"
#include "FadeManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Game.h"
#include "System/Input.h"
#include<Dxlib.h>
#include<algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//選択肢の座標
	constexpr int kResumePosX = 200;
	constexpr int kResumePosY = 140;
	constexpr int kBackTitlePosX = 200;
	constexpr int kBackTitlePosY = 200;

	//ボタンUIの座標
	constexpr int kButtonXPosX = 500;
	constexpr int kButtonXPosY = 140;
	constexpr int kButtonYPosX = 500;
	constexpr int kButtonYPosY = 200;
	constexpr int kButtonAPosX = 500;
	constexpr int kButtonAPosY = 260;
	constexpr int kButtonLPosX = 600;
	constexpr int kButtonLPosY = 140;
	constexpr int kButtonRPosX = 600;
	constexpr int kButtonRPosY = 200;

	//アニメーション切り替え間隔
	constexpr int kButtonAnimeInterval = 20;
}

PauseScene::PauseScene(SceneManager& sceneManager) :
	Scene(sceneManager),
	update_(&PauseScene::NormalUpdate),
	draw_(&PauseScene::NormalDraw),
	frameCount_(kFadeInterval),
	currentIndex_(0),
	isInputEnabled_(true)
{
}

PauseScene::~PauseScene()
{
}

void PauseScene::Init()
{
	//画像の初期化
	std::vector<int> handlesX;
	handlesX.push_back(LoadGraph(L"data/UI/Button/X1.png"));
	handlesX.push_back(LoadGraph(L"data/UI/Button/X2.png"));
	buttonX_.Init(kButtonXPosX, kButtonXPosY, handlesX, 0.25f, kButtonAnimeInterval);

	std::vector<int> handlesY;
	handlesY.push_back(LoadGraph(L"data/UI/Button/Y1.png"));
	handlesY.push_back(LoadGraph(L"data/UI/Button/Y2.png"));
	buttonY_.Init(kButtonYPosX, kButtonYPosY, handlesY, 0.25f, kButtonAnimeInterval);

	std::vector<int> handlesA;
	handlesA.push_back(LoadGraph(L"data/UI/Button/A1.png"));
	handlesA.push_back(LoadGraph(L"data/UI/Button/A2.png"));
	buttonA_.Init(kButtonAPosX, kButtonAPosY, handlesA, 0.25f, kButtonAnimeInterval);

	//L,Rボタンは静止画としてそのまま読み込む
	int handleL = LoadGraph(L"data/UI/Button/LB.png");
	buttonL_.Init(kButtonLPosX, kButtonLPosY, handleL,0.25f);

	int handleR = LoadGraph(L"data/UI/Button/RB.png");
	buttonR_.Init(kButtonRPosX, kButtonRPosY, handleR, 0.25f);
}

void PauseScene::Update()
{
	//ボタンUIの更新
	buttonX_.Update();
	buttonY_.Update();
	buttonA_.Update();
	buttonL_.Update();
	buttonR_.Update();

	(this->*update_)();
}

void PauseScene::Draw()
{
	(this->*draw_)();
}

void PauseScene::FadeInUpdate()
{
	frameCount_--;

	if (frameCount_ <= 0)
	{
		update_ = &PauseScene::NormalUpdate;
		draw_ = &PauseScene::NormalDraw;
		isInputEnabled_ = true;
		return;
	}
}

void PauseScene::NormalUpdate()
{
	if (!isInputEnabled_)return;

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
		SoundManager::GetInstance().PlaySe(SE::Decide);

		//ゲームシーンに戻る際はフェードせずにそのまま戻る
		//0の場合
		if (currentIndex_ == 0)
		{
			//このシーンを削除
			sceneManager_.PopScene();
		}

		//それ以外はフェードでシーン遷移を行う
		isInputEnabled_ = false;
		update_ = &PauseScene::FadeOutUpdate;
		draw_ = &PauseScene::FadeDraw;
		frameCount_ = kFadeInterval;
		return;
	}
}

void PauseScene::FadeOutUpdate()
{
	frameCount_--;

	if (frameCount_ < 0)
	{
		//タイトルを選んでいた場合のみタイトルへ遷移
		if (currentIndex_ == 1)
		{
			sceneManager_.ResetScene(std::make_shared<TitleScene>(sceneManager_));
			return;
		}
	}
}

void PauseScene::FadeDraw()
{
	float rate;

	if (update_ == &PauseScene::FadeInUpdate)
	{
		//フェードイン
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//フェードアウト
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}

	rate = std::clamp(rate, 0.0f, 1.0f);

	NormalDraw();

	//フェードマネージャーの描画開始と終了
	FadeManager::GetInstance().StartCapture();
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	FadeManager::GetInstance().EndCaptureAndDraw(rate);
}

void PauseScene::NormalDraw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	int resumeColor = (currentIndex_ == 0) ? 0xff0000 : 0xffffff;
	int titleColor = (currentIndex_ == 1) ? 0xff0000 : 0xffffff;

	const int centerX = Game::kScreenWidth / 2;
	const int centerY = Game::kScreenHeight / 2;

	const wchar_t* resumeText = L"ゲームにもどる";
	int resumeWidth = GetDrawStringWidthToHandle(resumeText, static_cast<int>(wcslen(resumeText)), Game::kFontUIHandle);
	DrawFormatStringToHandle(centerX - resumeWidth / 2, centerY, resumeColor, Game::kFontUIHandle, resumeText);

	const wchar_t* titleText = L"タイトルにもどる";
	int titleWidth = GetDrawStringWidthToHandle(titleText, static_cast<int>(wcslen(titleText)), Game::kFontUIHandle);
	DrawFormatStringToHandle(centerX - titleWidth / 2, centerY + 50, titleColor, Game::kFontUIHandle, titleText);

	//ボタンUIの描画
	buttonX_.Draw();
	buttonY_.Draw();
	buttonA_.Draw();
	buttonL_.Draw();
	buttonR_.Draw();
}