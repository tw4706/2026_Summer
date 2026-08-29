#include "ResultScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "System/Input.h"
#include "FadeManager.h"
#include "SoundManager.h"
#include<Dxlib.h>
#include<memory>
#include<cmath>
#include<algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//ランクのタイム基準となる値
	constexpr float kRankSTime = 60.0f;
	constexpr float kRankATime = 120.0f;
	constexpr float kRankBTime = 180.0f;

	constexpr int kTimeCountDuration = 60;   //タイムをカウントし終わるまでのフレーム数

	constexpr int kRankScaleDuration = 20;   //ランク拡大にかけるフレーム数
	constexpr float kRankScaleMax = 2.0f;    //ランクの最大拡大率

	//選択肢の座標
	constexpr int kRetryPosX = 0;
	constexpr int kRetryPosY = 60;
	constexpr int kBackTitlePosX = 0;
	constexpr int kBackTitlePosY = 120;

	//GameOverの文字のY座標オフセット
	constexpr int kGameOverOffsetY = 30;

	//クリア時間のY座標オフセット
	constexpr int kClearTimeOffsetY = 100;

	// 演出のタイミング
	constexpr int kClearTimeShowFrame = 0;		//クリアタイムを出すフレーム
	constexpr int kRankShowFrame = 30;			//ランクを出すフレーム
	constexpr int kButtonSlideStartFrame = 60;	//ボタンのスライドを始めるフレーム
	constexpr int kButtonSlideDuration = 20;	//スライドするフレーム
	constexpr int kButtonSlideDistance = 100;	//スライドのボタンの距離
}

ResultScene::ResultScene(SceneManager& sceneManager,float clearTime,bool isGameOver) :
	Scene(sceneManager),
	update_(&ResultScene::FadeInUpdate),
	draw_(&ResultScene::FadeDraw),
	frameCount_(kFadeInterval),
	clearTime_(clearTime),
	isGameOver_(isGameOver)
{
	if (isGameOver_)return;

	//ランク判定
	if (clearTime_ <= kRankSTime)
	{
		rank_ = L'S';
	}
	else if (clearTime_ <= kRankATime)
	{
		rank_ = L'A';
	}
	else if (clearTime_ <= kRankBTime)
	{
		rank_ = L'B';
	}
	else
	{
		rank_ = L'C';
	}
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init()
{
	pBg_ = std::make_shared<Bg>();
	pBg_->Init(L"data/Bg/backGroundResult");

	SoundManager::GetInstance().PlayBgm(BGM::Result);
}

void ResultScene::Update()
{
	(this->*update_)();
}

void ResultScene::Draw()
{
	(this->*draw_)();
}

void ResultScene::FadeInUpdate()
{
	frameCount_--;

	if (frameCount_ <= 0)
	{
		update_ = &ResultScene::NormalUpdate;
		draw_ = &ResultScene::NormalDraw;
	}
}

void ResultScene::NormalUpdate()
{
	//演出中は入力を受け付けず、カウンタを進める
	if (!isInputEnabled_)
	{
		performanceCount_++;
		if (performanceCount_ >= kButtonSlideStartFrame + kButtonSlideDuration)
		{
			isInputEnabled_ = true;
		}
		return;
	}

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
		update_ = &ResultScene::FadeOutUpdate;
		draw_ = &ResultScene::FadeDraw;
		frameCount_ = kFadeInterval;
		SoundManager::GetInstance().PlaySe(SE::Decide);
	}
}

void ResultScene::FadeOutUpdate()
{
	frameCount_--;

	if (frameCount_ <= 0)
	{
		if (currentIndex_ == 0)
		{
			sceneManager_.ChangeScene(std::make_shared<GameScene>(sceneManager_));
		}
		else
		{
			sceneManager_.ChangeScene(std::make_shared<TitleScene>(sceneManager_));
		}
	}
}

void ResultScene::FadeDraw()
{
	float rate;

	if (update_ == &ResultScene::FadeInUpdate)
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

	//フェードマネージャーの描画開始と終了
	FadeManager::GetInstance().StartCapture();
	NormalDraw();
	FadeManager::GetInstance().EndCaptureAndDraw(rate);
}

void ResultScene::NormalDraw()
{
	int retryColor = (currentIndex_ == 0) ? 0xff0000 : 0x000000;
	int titleColor = (currentIndex_ == 1) ? 0xff0000 : 0x000000;
	
	//背景描画
	pBg_->Draw(Vector3{ 0.0f, 0.0f, 0.0f });

	const int centerX = Game::kScreenWidth / 2;
	const int centerY = Game::kScreenHeight / 2;

	if (isGameOver_)
	{
		const wchar_t* text = L"GameOver";
		int textWidth = GetDrawStringWidthToHandle(text, static_cast<int>(wcslen(text)), Game::kFontUIHandle);

		DrawFormatStringToHandle(centerX - textWidth / 2, centerY - kGameOverOffsetY, 0x000000, Game::kFontUIHandle, text);
	}
	else
	{
		if (performanceCount_ >= kClearTimeShowFrame)
		{
			//カウントアップの計算
			float t = std::clamp(static_cast<float>(performanceCount_ - kClearTimeShowFrame) / kTimeCountDuration, 0.0f, 1.0f);
			displayedTime_ = clearTime_ * t;

			wchar_t timeBuffer[64];
			swprintf_s(timeBuffer, L"クリアタイム: %.1f秒", displayedTime_);
			int textWidth = GetDrawStringWidthToHandle(timeBuffer, static_cast<int>(wcslen(timeBuffer)), Game::kFontUIHandle);

			DrawFormatStringToHandle(centerX - textWidth / 2, centerY - kClearTimeOffsetY, 0x000000, Game::kFontUIHandle, timeBuffer);
		}

		//ランクはタイムのカウントが終わってから出す
		if (performanceCount_ >= kRankShowFrame)
		{
			//拡大率の計算
			float rt = std::clamp(static_cast<float>(performanceCount_ - kRankShowFrame) / kRankScaleDuration, 0.0f, 1.0f);
			float rankScale = kRankScaleMax * (1.0f - std::pow(1.0f - rt, 3.0f));

			wchar_t rankBuffer[8];
			swprintf_s(rankBuffer, L"%c", rank_);

			int baseWidth = GetDrawStringWidthToHandle(rankBuffer, static_cast<int>(wcslen(rankBuffer)), Game::kFontUIHandle);
			int drawX = centerX - static_cast<int>(baseWidth * rankScale / 2);

			DrawExtendFormatStringToHandle(drawX, centerY, rankScale, rankScale, 0x000000, Game::kFontUIHandle, rankBuffer);
		}
	}

	// ボタンのスライド計算
	float t = std::clamp(static_cast<float>(performanceCount_ - kButtonSlideStartFrame) / kButtonSlideDuration, 0.0f, 1.0f);
	float eased = 1.0f - std::pow(1.0f - t, 3.0f);
	int slideOffset = static_cast<int>((1.0f - eased) * kButtonSlideDistance);

	if (performanceCount_ >= kButtonSlideStartFrame)
	{
		// リトライ
		const wchar_t* retryText = L"リトライ";
		int retryWidth = GetDrawStringWidthToHandle(retryText, static_cast<int>(wcslen(retryText)), Game::kFontUIHandle);
		DrawFormatStringToHandle(centerX - retryWidth / 2 + kRetryPosX, centerY + kRetryPosY - slideOffset, retryColor, Game::kFontUIHandle, retryText);

		// タイトルに戻る
		const wchar_t* titleText = L"タイトルに戻る";
		int titleWidth = GetDrawStringWidthToHandle(titleText, static_cast<int>(wcslen(titleText)), Game::kFontUIHandle);
		DrawFormatStringToHandle(centerX - titleWidth / 2 + kBackTitlePosX, centerY + kBackTitlePosY - slideOffset, titleColor, Game::kFontUIHandle, titleText);
	}
}
