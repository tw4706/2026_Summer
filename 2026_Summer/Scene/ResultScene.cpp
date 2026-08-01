#include "ResultScene.h"
#include "TitleScene.h"
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

	//タイトルに戻る選択肢の座標
	constexpr int kBackTitlePosX = 0;
	constexpr int kBackTitlePosY = 30;
}

ResultScene::ResultScene(SceneManager& sceneManager):
	Scene(sceneManager),
	update_(&ResultScene::FadeInUpdate),
	draw_(&ResultScene::FadeDraw),
	frameCount_(kFadeInterval)
{
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
		sceneManager_.ChangeScene(std::make_shared<TitleScene>(sceneManager_));
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

	FadeManager::GetInstance().StartCapture();
	NormalDraw();
	FadeManager::GetInstance().EndCaptureAndDraw(rate);
}

void ResultScene::NormalDraw()
{
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenWidth, 0xffffff, true);
	DrawFormatString(0, 0, 0x000000, L"リザルトシーン");
	DrawFormatString(kBackTitlePosX, kBackTitlePosY, 0x000000, L"ボタンを押してタイトルに戻る");
}
