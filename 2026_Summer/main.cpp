#include "DxLib.h"
#include "Game.h"
#include "Input.h"
#include "GameScene.h"
#include <memory>



// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ウィンドウモード設定
	ChangeWindowMode(true);
	// ウィンドウのタイトル変更
	SetMainWindowText("ゲーム名");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	

	SetDrawScreen(DX_SCREEN_BACK);

	//シーンの作成
	Input input;
	std::shared_ptr<GameScene>pScene = std::make_shared<GameScene>();
	pScene->Init(input);
	

	
	while (ProcessMessage() != -1)
	{
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレーム描画を消す
		ClearDrawScreen();
		//（ゲーム内容）
		input.Update();
		pScene->Update(input);
		pScene->Draw();

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// 内容を画面に反映
		ScreenFlip();

		// ゲーム速度がどのPCでも同じスピードになる
		while (GetNowHiPerformanceCount() - start < 16667)
		{

		}
	}

	// メモリの開放
	// shared_ptrはメモリの開放を自動で行ってくれる

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}