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
#include "Camera/TitleCamera.h"
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

	//テキストの影のX座標オフセット
	constexpr int kShadowTextOffsetX = 4;

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

	//プレイヤーの移動速度
	constexpr float kPlayerMoveSpeed = 5.0f;

	//固定カメラのままプレイヤーを走らせておくフレーム数
	constexpr int kIntroHoldDuration = 60;
}

TitleScene::TitleScene(SceneManager& sceneManager) :
	Scene(sceneManager),
	update_(&TitleScene::FadeInUpdate),
	draw_(&TitleScene::FadeDraw),
	frameCount_(kFadeInterval)
{
	pBg_ = std::make_shared<Bg>();
	pStage_ = std::make_shared<Stage>(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, 0.0f);
	pTitlePlayer_ = std::make_shared<TitlePlayer>();
	pTitleCamera_ = std::make_shared<TitleCamera>();
}

TitleScene::~TitleScene()
{
	//ハンドルの削除
	DeleteGraph(titleLogoHandle_);
	DeleteGraph(startTextHandle_);
	DeleteGraph(endTextHandle_);
	DeleteGraph(startShadowTextHandle_);
	DeleteGraph(endShadowTextHandle_);
}

void TitleScene::Init()
{
	frameCount_ = kFadeInterval;

	//ハンドルの読み込み
	titleLogoHandle_ = LoadGraph(L"data/UI/titleLogo.png");
	startTextHandle_ = LoadGraph(L"data/UI/start.png");
	endTextHandle_ = LoadGraph(L"data/UI/endText.png");
	startShadowTextHandle_ = LoadGraph(L"data/UI/start_white.png");
	endShadowTextHandle_ = LoadGraph(L"data/UI/end_white.png");

	//ハンドルサイズの取得
	GetGraphSize(startTextHandle_, &startTextWidth_, &startTextHeight_);
	GetGraphSize(endTextHandle_, &endTextWidth_, &endTextHeight_);

	//背景の初期化
	pBg_->Init((L"data/Bg/backGround"));

	//ステージの初期化
	pStage_->Init();

	pTitlePlayer_->Init();
	pTitleCamera_->Init();

	Vector3 dir = pTitlePlayer_->GetForward();
	pTitlePlayer_->StartRun(dir, kPlayerMoveSpeed);
	pTitleCamera_->SetupIntro(pTitlePlayer_->GetPos(), dir);

	SoundManager::GetInstance().PlayBgm(BGM::Title);
}

void TitleScene::Update()
{
	//フェードアウト中かどうか
	//このフラグを用いることによってフェードアウト中でもカメラの固定を行えるようにする
	bool isFadingOut = (update_ == &TitleScene::FadeOutUpdate);

	if (pTitlePlayer_)
	{
		pTitlePlayer_->Update();

		//フェードアウト中でなければカメラ追従を行う
		if (!isFadingOut)
		{
			pTitleCamera_->FollowPlayer(pTitlePlayer_->GetPos());
		}
	}

	//フェードアウト中でなければカメラ本体の更新を行う
	if (!isFadingOut)
	{
		pTitleCamera_->Update(0);
	}

	//固定のまま一定フレーム走らせたら、カメラを背後へ引き始める
	if (!isPullBackStarted_)
	{
		introHoldFrameCount_++;
		if (introHoldFrameCount_ >= kIntroHoldDuration)
		{
			pTitleCamera_->StartTransitionToFollow();
			isPullBackStarted_ = true;
		}
	}

	//完全追従になった＝背後に収まった瞬間を検知
	if (!isFollowStarted_ && pTitleCamera_->IsFollowing())
	{
		isFollowStarted_ = true;
		pTitlePlayer_->StopRun();

		//ここで引きカメラの開始を行う
		pTitleCamera_->StartPullBackForLogo();
	}

	//演出スキップ
	if (!isFollowStarted_ && Input::GetInstance().IsTriggered("next"))
	{
		pTitlePlayer_->StopRun();
		pTitleCamera_->SkipFollow(pTitlePlayer_->GetPos());

		isPullBackStarted_ = true;
		isFollowStarted_ = true;

		//このまま決定を押すと同時にフェードアウトが始まってしまうので、returnで止める
		return;
	}

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
	//背景の更新
	pBg_->Update();

	if (!isFollowStarted_) return;

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

	//現在のカメラの座標を固定するために更新処理を置く
	if (pTitleCamera_)
	{
		pTitleCamera_->Update(0);
	}

	NormalDraw();
	FadeManager::GetInstance().EndCaptureAndDraw(rate);
}

void TitleScene::NormalDraw()
{
	//背景の描画
	if (pBg_)
	{
		pBg_->Draw(Vector3{ 0.0f,0.0f,0.0f });
	}

	//ステージの描画
	if (pStage_)
	{
		pStage_->Draw();
	}

	//演出中のみプレイヤーの描画を行う
	if (!isFollowStarted_)
	{
		if (pTitlePlayer_)
		{
			pTitlePlayer_->Draw();
		}

		//演出が終わっていない場合はUIを描画せずに終了
		return;
	}

	float startScale = (currentIndex_ == 0) ? kSelectedScale : kUnselectedScale;
	float endScale = (currentIndex_ == 1) ? kSelectedScale : kUnselectedScale;

	startCurrentScale_ = Vector3::Lerp(startCurrentScale_, startScale, kScaleLerpRate);
	endCurrentScale_ = Vector3::Lerp(endCurrentScale_, endScale, kScaleLerpRate);

	DrawRotaGraph3(Game::kScreenWidth / 2+ kShadowTextOffsetX, Game::kScreenHeight / 2 + kTitleTextOffsetY,
		startTextWidth_ / 2, startTextHeight_ / 2, startCurrentScale_, startCurrentScale_, 0.0f, startShadowTextHandle_, true);
	DrawRotaGraph3(Game::kScreenWidth / 2, Game::kScreenHeight / 2 + kTitleTextOffsetY,
		startTextWidth_ / 2, startTextHeight_ / 2, startCurrentScale_, startCurrentScale_, 0.0f, startTextHandle_, true);

	DrawRotaGraph3(Game::kScreenWidth / 2+ kShadowTextOffsetX, Game::kScreenHeight / 2 + kEndTextOffsetY,
		endTextWidth_ / 2, endTextHeight_ / 2, endCurrentScale_, endCurrentScale_, 0.0f, endShadowTextHandle_, true);
	DrawRotaGraph3(Game::kScreenWidth / 2, Game::kScreenHeight / 2 + kEndTextOffsetY,
		endTextWidth_ / 2, endTextHeight_ / 2, endCurrentScale_, endCurrentScale_, 0.0f, endTextHandle_, true);

	//タイトルロゴの描画
	DrawRotaGraph3(Game::kScreenWidth / 2 + kLogoOffsetX, Game::kScreenHeight / 2 + kLogoOffsetY, 0, 0,
		kLogoScaleX, kLogoScaleY, 0.0f, titleLogoHandle_, true);
}