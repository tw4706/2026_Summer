#include "GameScene.h"
#include "DxLib.h"
#include "Player/Player.h"
#include "Camera/CameraManager.h"
#include "Camera/PlayerCamera.h"
#include "GameObject.h"
#include"Input.h"
#include <algorithm>

namespace
{
}

GameScene::GameScene() :
	frameCount_(0)
{
	pPlayer_ = std::make_shared<Player>();
	pCameraManager_ = std::make_unique<CameraManager>();

	//カメラの登録
	auto playerCamera = std::make_shared<PlayerCamera>();
	pCameraManager_->RegisterCamera("PlayerCamera", playerCamera);

	//ゲームオブジェクトの登録
	//プレイヤーの登録
	RegisterGameObject(pPlayer_);
}

GameScene::~GameScene()
{

}

void GameScene::Init(Input&input)
{
	//カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(true);

	//Zバッファの設定
	SetUseZBuffer3D(true);		//Zバッファを使う
	SetWriteZBuffer3D(true);	//Zバッファ書き込み

	//現在のアクティブカメラを取得
	auto activeCam = pCameraManager_->GetActiveCamera();

	pPlayer_->SetInput(&input);
	pPlayer_->SetCamera(activeCam.get());
	pPlayer_->Init();

	//プレイヤーカメラへのセット
	auto playerCam = std::dynamic_pointer_cast<PlayerCamera>(activeCam);
	if (playerCam)
	{
		playerCam->SetPlayer(pPlayer_);
		playerCam->Init();
	}
}

void GameScene::Update(Input& input)
{
	frameCount_++;

	if (!reserveObjList_.empty())
	{
		//insertはinsert(追加したい場所,追加したいデータの先頭、追加したいデータの末尾)
		gameObjects_.insert(gameObjects_.end(), reserveObjList_.begin(), reserveObjList_.end());
		reserveObjList_.clear();

		//オブジェクトを指定した優先度順で並び替える
		std::sort(gameObjects_.begin(), gameObjects_.end(), [](const auto& a, const auto& b)
			{
				return a->GetPriority() < b->GetPriority();
			});
	}

	//カメラマネージャーの更新
	pCameraManager_->Update(input);

	//すべてのゲームオブジェクトの更新
	for (auto& obj : gameObjects_) {
		if (!obj->IsDead())
		{
			obj->Update();
		}
	}

	//死んでいるゲームオブジェクトの削除
	gameObjects_.erase(
		std::remove_if(gameObjects_.begin(), gameObjects_.end(), [](const auto& obj) {
			return obj->IsDead();
			}),
		gameObjects_.end()
	);
}

void GameScene::Draw()
{
	DrawGrid();

	//すべてのゲームオブジェクトの描画
	for (auto& obj : gameObjects_)
	{
		if (!obj->IsDead())
		{
			obj->Draw();
		}
	}

#ifdef _DEBUG
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", frameCount_);
#endif
}

void GameScene::RegisterGameObject(std::shared_ptr<GameObject> obj)
{
	//予約リストへの追加
	reserveObjList_.push_back(obj);
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
