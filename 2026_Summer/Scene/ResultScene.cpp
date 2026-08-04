#include "ResultScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "System/Input.h"
#include "FadeManager.h"
#include<Dxlib.h>
#include<memory>
#include<algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//ランクのタイム基準となる値
	constexpr float kRankSTime = 60.0f;
	constexpr float kRankATime = 120.0f;
	constexpr float kRankBTime = 180.0f;

	//選択肢の座標
	constexpr int kRetryPosX = 0;
	constexpr int kRetryPosY = 30;
	constexpr int kBackTitlePosX = 0;
	constexpr int kBackTitlePosY = 60;
}

ResultScene::ResultScene(SceneManager& sceneManager,float clearTime) :
	Scene(sceneManager),
	update_(&ResultScene::FadeInUpdate),
	draw_(&ResultScene::FadeDraw),
	frameCount_(kFadeInterval),
	clearTime_(clearTime)
{
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
		update_ = &ResultScene::FadeOutUpdate;
		draw_ = &ResultScene::FadeDraw;
		frameCount_ = kFadeInterval;
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

	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xffffff, true);
	DrawFormatString(0, 0, 0x000000, L"リザルトシーン");
	DrawFormatString(Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 30, 0x000000, L"クリアタイム: %.1f秒", clearTime_);
	DrawFormatString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 0x000000, L"ランク: %c", rank_);

	DrawFormatString(Game::kScreenWidth / 2 + kRetryPosX, Game::kScreenHeight / 2 + kRetryPosY, retryColor, L"リトライ");
	DrawFormatString(Game::kScreenWidth / 2 + kBackTitlePosX, Game::kScreenHeight / 2 + kBackTitlePosY, titleColor, L"タイトルに戻る");
}
