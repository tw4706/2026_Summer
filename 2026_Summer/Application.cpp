#include "Application.h"
#include "Input.h"
#include "Game.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "EffekseerForDXLib.h"
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

	//------------------------------//
		// エフェクトの初期化
		//------------------------------//
	{
		// DirectX9を使用するようにする。(DirectX11も可)
		// Effekseerを使用するには必ず設定する。
		SetUseDirect3DVersion(DX_DIRECT3D_11);

		// Effekseerを初期化する。
		// 引数には画面に表示する最大パーティクル数を設定する。
		if (Effkseer_Init(2000) == -1)
		{
			DxLib_End();
			return false;
		}

		// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
		// Effekseerを使用する場合は必ず設定する。
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

		// DXライブラリのデバイスロストした時のコールバックを設定する。
		// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
		// ただし、DirectX11を使用する場合は実行する必要はない。
		Effekseer_SetGraphicsDeviceLostCallbackFunctions();

		// Effekseerに2D描画の設定をする。
		Effekseer_Set2DSetting(Game::kScreenWidth, Game::kScreenHeight);

		// Zバッファを有効にする。
		// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
		SetUseZBuffer3D(TRUE);

		// Zバッファへの書き込みを有効にする。
		// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
		SetWriteZBuffer3D(TRUE);
	}
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
		UpdateEffekseer3D();
		sceneManager.Draw();
		DrawEffekseer3D();

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
	Effkseer_End();

	//メモリの開放
	DxLib_End();				//ＤＸライブラリ使用の終了処理
}
