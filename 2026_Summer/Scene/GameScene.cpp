#include "GameScene.h"
#include "DxLib.h"
#include "Player/Player.h"
#include "Camera.h"

namespace
{
}

GameScene::GameScene() :
	m_frameCount(0)
{
	pPlayer_ = std::make_shared<Player>();
	pCamera_ = std::make_shared<Camera>();
}

GameScene::~GameScene()
{
	
}

void GameScene::Init()
{
	//カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(true);

	//Zバッファの設定
	SetUseZBuffer3D(true);		//Zバッファを使う
	SetWriteZBuffer3D(true);	//Zバッファ書き込み

	pPlayer_->SetCamera(pCamera_.get());
	pPlayer_->Init();

	pCamera_->SetPlayer(pPlayer_);
	pCamera_->Init();
}

void GameScene::Update(Input&input)
{
	m_frameCount++;

	pPlayer_->Update(input);
	pCamera_->Update();
}

void GameScene::Draw()
{
	DrawGrid();

	pPlayer_->Draw();

	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", m_frameCount);
}

void GameScene::DrawGrid()
{
	// 直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

	for (int z = -300; z <= 300; z += 100)
	{
		startPos = VGet(-300.0f, 0.0f, static_cast<float>(z));
		endPos = VGet(300.0f, 0.0f, static_cast<float>(z));
		DrawLine3D(startPos, endPos, 0xff0000);
	}
	for (int x = -300; x <= 300; x += 100)
	{
		startPos = VGet(static_cast<float>(x), 0.0f, -300.0f);
		endPos = VGet(static_cast<float>(x), 0.0f, 300.0f);
		DrawLine3D(startPos, endPos, 0x0000ff);
	}
}
