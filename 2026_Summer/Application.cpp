#include "Application.h"
#include "Input.h"
#include "Game.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include<Dxlib.h>
#include<memory>

Application::Application()
{
}
Application::~Application()
{
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

bool Application::Init()
{
	//ウィンドウモード設定
	ChangeWindowMode(true);
	//ウィンドウのタイトル変更
	SetMainWindowText(L"SamuraiAdapt");
	//画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		//ＤＸライブラリ初期化処理
	{
		return -1;				//エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK);

	//Zバッファの設定
	SetUseZBuffer3D(true);		//Zバッファを使う
	SetWriteZBuffer3D(true);	//Zバッファ書き込み

	return true;
}

void  Application::Run()
{
	//シーンの作成
	SceneManager sceneManager;

	//シーンの切り替え
	sceneManager.ChangeScene(std::make_shared<TitleScene>(sceneManager));

	while (ProcessMessage() != -1)
	{
		LONGLONG start = GetNowHiPerformanceCount();

		//前のフレーム描画を削除
		ClearDrawScreen();
		Input::GetInstance().Update();
		sceneManager.Update();
		sceneManager.Draw();

		if (isGameEnd_||CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		//画面に反映
		ScreenFlip();

		//ゲーム速度がどのPCでも同じスピードになる
		while (GetNowHiPerformanceCount() - start < 16667)
		{

		}
	}
}

void Application::Terminate()
{
	//メモリの開放
	DxLib_End();				//ＤＸライブラリ使用の終了処理
}
