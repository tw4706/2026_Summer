#include "GameScene.h"
#include "DxLib.h"
#include "Player/Player.h"
#include "Camera/CameraManager.h"
#include "Camera/PlayerCamera.h"
#include"Enemy/Oni.h"
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

	//敵の登録
	auto pOni = std::make_shared<Oni>();
	RegisterGameObject(pOni);

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

	//現在のアクティブカメラを取得
	auto activeCam = pCameraManager_->GetActiveCamera();

	//プレイヤーカメラへのセット
	auto playerCam = std::dynamic_pointer_cast<PlayerCamera>(activeCam);
	if (playerCam)
	{
		playerCam->SetPlayer(pPlayer_);
		playerCam->Init();
	}

	if (!reserveObjList_.empty())
	{
		gameObjects_.insert(gameObjects_.end(), reserveObjList_.begin(), reserveObjList_.end());
		reserveObjList_.clear();
	}

	// 登録されたすべてのオブジェクトを初期化
	for (auto& obj : gameObjects_)
	{
		if (auto player = std::dynamic_pointer_cast<Player>(obj)) {

			player->SetInput(&input);
			player->SetCamera(playerCam.get());
			player->Init();
		}
		else if (auto oni = std::dynamic_pointer_cast<Oni>(obj))
		{
			oni->SetPlayer(pPlayer_);
			oni->Init(); //鬼の初期化
		}
	}

}

void GameScene::Update(Input& input)
{
	frameCount_++;

	if (!reserveObjList_.empty())
	{
		gameObjects_.insert(gameObjects_.end(), reserveObjList_.begin(), reserveObjList_.end());
		reserveObjList_.clear();

		//描画優先度順で並び替える
		std::sort(gameObjects_.begin(), gameObjects_.end(), [](const auto& a, const auto& b)
			{
				return a->GetPriority() < b->GetPriority();
			});
	}
	auto playerCam = std::dynamic_pointer_cast<PlayerCamera>(pCameraManager_->GetActiveCamera());
	if (playerCam)
	{
		Vector3 stickR = input.GetStickRight();
		playerCam->AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);
	}

	//カメラマネージャーの更新
	pCameraManager_->Update();

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

	for (auto& obj : gameObjects_)
	{
		if (!obj->IsDead())
		{
			obj->Draw();
		}
	}

	DrawGrid();

#ifdef _DEBUG
	DrawString(0, 0, L"SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), L"FRAME:%d", frameCount_);
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
