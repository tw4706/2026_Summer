#include "ResultScene.h"
#include "TitleScene.h"
#include "Game.h"
#include "System/Input.h"
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
	if (frameCount_-- <= 0)
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
	}
}

void ResultScene::FadeOutUpdate()
{
	if (frameCount_-- <= 0)
	{
		sceneManager_.ChangeScene(std::make_shared<TitleScene>(sceneManager_));
	}
}

void ResultScene::FadeDraw()
{
	NormalDraw();

	float rate;

	if (update_ == &ResultScene::FadeInUpdate)
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

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ResultScene::NormalDraw()
{
	DrawFormatString(0, 0, 0xffffff, L"リザルトシーン");
	DrawFormatString(kBackTitlePosX, kBackTitlePosY, 0xffffff, L"ボタンを押してタイトルに戻る");
}
