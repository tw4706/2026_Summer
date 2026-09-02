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
}

void PauseScene::Update()
{
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
}