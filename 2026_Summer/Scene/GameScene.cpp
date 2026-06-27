#include "GameScene.h"
#include"Stage.h"
#include"Input.h"
#include"Katana.h"
#include"Enemy/Oni.h"
#include "GameObject.h"
#include "Player/Player.h"
#include "CollisionManager.h"
#include "Camera/PlayerCamera.h"
#include "Camera/CameraManager.h"
#include <DxLib.h>
#include <algorithm>

GameScene::GameScene() :
	frameCount_(0)
{
	pCollisionManager_ = std::make_unique<CollisionManager>();

	pPlayer_ = std::make_shared<Player>();
	pCameraManager_ = std::make_unique<CameraManager>();

	//ゲームオブジェクトの登録
	//カメラの登録
	auto playerCamera = std::make_shared<PlayerCamera>();
	pCameraManager_->RegisterCamera("PlayerCamera", playerCamera);

	//ステージの登録
	pStage_ = std::make_shared<Stage>(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f);
	RegisterGameObject(pStage_);

	//敵の登録
	pOni_ = std::make_shared<Oni>();
	RegisterGameObject(pOni_);

	//プレイヤーの登録
	RegisterGameObject(pPlayer_);


}

GameScene::~GameScene()
{
	gameObjects_.clear();
	reserveObjList_.clear();
	pPlayer_ = nullptr;
	pOni_ = nullptr;
}

void GameScene::Init(Input& input)
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
		if (auto stage = std::dynamic_pointer_cast<Stage>(obj))
		{
			stage->Init();

			//コライダーの登録
			for (const auto& pCollider : stage->GetColliders())
			{
				pCollisionManager_->RegisterCollider(pCollider.get());
			}
		}
		else if (auto player = std::dynamic_pointer_cast<Player>(obj)) {

			player->SetInput(&input);
			player->SetCamera(playerCam.get());
			player->Init();

			//コライダーの登録
			for (const auto& pCollider : player->GetColliders())
			{
				pCollisionManager_->RegisterCollider(pCollider.get());
			}

			//刀の登録
			if (auto pKatana = player->GetKatana())
			{
				for (const auto& pCollider : pKatana->GetColliders())
				{
					pCollisionManager_->RegisterCollider(pCollider.get());
				}
			}
		}
		else if (auto oni = std::dynamic_pointer_cast<Oni>(obj))
		{
			oni->SetPlayer(pPlayer_);
			oni->Init(); //鬼の初期化

			//コライダーの登録
			for (const auto& pCollider : oni->GetColliders())
			{
				pCollisionManager_->RegisterCollider(pCollider.get());
			}
		}

	}

}

void GameScene::Update(Input& input)
{
	frameCount_++;

	if (!reserveObjList_.empty())
	{
		// 結合する前に、新しく入ってきたオブジェクトのコライダーをここで確実に登録する！
		for (auto& obj : reserveObjList_)
		{
			if (auto collidableObj = std::dynamic_pointer_cast<Collidable>(obj))
			{
				for (const auto& pCollider : collidableObj->GetColliders())
				{
					pCollisionManager_->RegisterCollider(pCollider.get());
				}
			}
		}

		gameObjects_.insert(gameObjects_.end(), reserveObjList_.begin(), reserveObjList_.end());
		reserveObjList_.clear();

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

	//すべてのゲームオブジェクトの更新
	for (auto& obj : gameObjects_)
	{

		if (!obj->IsDead())
		{
			obj->Update();
		}
	}

	//当たり判定の更新
	if (pCollisionManager_)
	{
		pCollisionManager_->UpdateCheckCollision();
	}

	//死んだオブジェクトのコライダーは、配列から消える前にコリジョンマネージャーから外す
	for (auto& obj : gameObjects_)
	{
		if (obj->IsDead())
		{
			//Collidable型にキャストできればコライダーを外す
			if (auto collidableObj = std::dynamic_pointer_cast<Collidable>(obj))
			{
				for (const auto& pCollider : collidableObj->GetColliders())
				{
					pCollisionManager_->UnRegisterCollider(pCollider.get());
				}
			}
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
