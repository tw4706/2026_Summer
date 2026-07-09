#include "GameScene.h"
#include"Stage.h"
#include"Input.h"
#include"Katana.h"
#include"Enemy/Oni.h"
#include"GameObject.h"
#include"EnemyManager.h"
#include"SceneManager.h"
#include"ResultScene.h"
#include"Enemy/BigMan.h"
#include"Player/Player.h"
#include"CollisionManager.h"
#include"Camera/PlayerCamera.h"
#include"Camera/LockOnCamera.h"
#include"Camera/CameraManager.h"
#include"Game.h"
#include <DxLib.h>
#include <algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;
}

GameScene::GameScene(SceneManager& sceneManager) :
	Scene(sceneManager),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw),
	frameCount_(kFadeInterval)
{
	pPlayer_ = std::make_shared<Player>();
	pCameraManager_ = std::make_unique<CameraManager>();
	pEnemyManager_ = std::make_unique<EnemyManager>();

	//ゲームオブジェクトの登録
	//カメラの登録
	auto playerCamera = std::make_shared<PlayerCamera>();
	pCameraManager_->RegisterCamera(L"PlayerCamera", playerCamera);
	auto lockOnCamera = std::make_shared<LockOnCamera>();
	pCameraManager_->RegisterCamera(L"LockOnCamera", lockOnCamera);

	//ステージの登録
	pStage_ = std::make_shared<Stage>(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f);
	RegisterGameObject(pStage_);

	//CSV読み込み
	pEnemyManager_->LoadEnemyData(L"data/CSV/EnemyData.csv");
	pEnemyManager_->LoadEnemySpawnData(L"data/CSV/EnemySpawnData.csv");
	pEnemyManager_->LoadWayPointData(L"data/CSV/WayPointData.csv");

	//プレイヤーの登録
	RegisterGameObject(pPlayer_);
}

GameScene::~GameScene()
{
	gameObjects_.clear();
	reserveObjList_.clear();
	pPlayer_ = nullptr;
}

void GameScene::Init()
{
	//カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(true);

	//現在のアクティブカメラを取得
	auto activeCam = pCameraManager_->GetActiveCamera();
	//プレイヤーカメラへのセット
	auto playerCam = std::dynamic_pointer_cast<PlayerCamera>(activeCam);

	if (!reserveObjList_.empty())
	{
		gameObjects_.insert(gameObjects_.end(), reserveObjList_.begin(), reserveObjList_.end());
		reserveObjList_.clear();
	}

	//登録されたすべてのオブジェクトを初期化
	for (auto& obj : gameObjects_)
	{
		if (auto stage = std::dynamic_pointer_cast<Stage>(obj))
		{
			stage->Init();
		}
		else if (auto player = std::dynamic_pointer_cast<Player>(obj))
		{
			player->SetCamera(playerCam.get());
			player->Init();
		}
	}

	//プレイヤーカメラの初期化
	if (playerCam)
	{
		playerCam->SetPlayer(pPlayer_);
		playerCam->Init();
	}

	//敵の生成
	for (int areaId = 0; areaId < 3; ++areaId)
	{
		auto enemies = pEnemyManager_->SpawnEnemyArea(areaId);

		for (auto& enemy : enemies)
		{
			enemy->SetPlayer(pPlayer_);
			enemy->SetNavigationGrid(pStage_->GetNaviGrid());
			enemy->SetStageModelHandle(pStage_->GetHandle());
		}
	}

	//カメラマネージャーの更新
	pCameraManager_->Update(pStage_->GetHandle());
}

void GameScene::Update()
{
	(this->*update_)();
}

void GameScene::Draw()
{
	(this->*draw_)();
}

void GameScene::FadeInUpdate()
{
	//すべてのゲームオブジェクトの更新
	for (auto& obj : gameObjects_)
	{

		if (!obj->IsDead())
		{
			obj->Update();
		}
	}

	//敵マネージャーの更新
	for (auto& enemy : pEnemyManager_->GetEnemies())
	{
		enemy->Update();
	}

	//カメラマネージャーの更新
	pCameraManager_->Update(pStage_->GetHandle());

	if (frameCount_-- <= 0)
	{
		update_ = &GameScene::NormalUpdate;
		draw_ = &GameScene::NormalDraw;
	}
}

void GameScene::NormalUpdate()
{
	frameCount_++;

	//プレイヤーが死んだ場合はシーン遷移をする
	if (pPlayer_->IsDead())
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frameCount_ = 0;
		return;
	}

	if (!reserveObjList_.empty())
	{
		//オブジェクトのコライダーを登録
		for (auto& obj : reserveObjList_)
		{
			if (auto collidableObj = std::dynamic_pointer_cast<Collidable>(obj))
			{
				for (const auto& pCollider : collidableObj->GetColliders())
				{
					CollisionManager::GetInstance().RegisterCollider(pCollider.get());
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

	//ロックオンカメラ
	if(Input::GetInstance().IsTriggered("lockOn"))
	{
		//現在アクティブなカメラがロックオンカメラの場合
		if (pCameraManager_->GetActiveCameraName() == L"LockOnCamera")
		{
			auto activeCam = pCameraManager_->GetActiveCamera();
			auto playerCam = pCameraManager_->GetCamera(L"PlayerCamera");

			auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(activeCam);
			auto playerCamera = std::dynamic_pointer_cast<PlayerCamera>(playerCam);

			// 両方のキャストに成功したら、角度の同期関数を呼び出す
			if (lockOnCamera && playerCamera)
			{
				playerCamera->SetRotationToLockOn(lockOnCamera->GetPos(), lockOnCamera->GetCameraTarget());
			}

			//通常のプレイヤーカメラに戻す
			//(ロックオンはすでに行っている状態なので)
			pCameraManager_->ChangeCamera(L"PlayerCamera");

			//ロックオンを解除
			pPlayer_->SetLockOn(false);
		}
		else
		{
			//floatの最大値を取ってロックオンの範囲を決める
			float closestDistanceSq = FLT_MAX;

			//一番近い敵のポインタを保存するポインタ
			std::shared_ptr<EnemyBase>closestEnemy;

			//敵分ループを回す
			for (auto& enemy : pEnemyManager_->GetEnemies())
			{
				if (enemy->IsDead())continue;

				//プレイヤーと敵の差分のベクトルを計算
				Vector3 diff = enemy->GetPos() - pPlayer_->GetPos();

				//距離の2情を計算
				float distSq = diff.LengthSq();

				//今見つけている敵より距離が短い場合
				if (distSq < closestDistanceSq)
				{
					closestDistanceSq = distSq;	//一番近い距離を更新
					closestEnemy = enemy;		//一番近い敵を更新
				}
			}

			//ロックオンできる敵がいるなら
			if (closestEnemy != nullptr)
			{
				//カメラ
				// マネージャーからカメラを取り出す
				auto camera=pCameraManager_->GetCamera(L"LockOnCamera");

				//ロックオンカメラの型にキャスト
				auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(camera);

				//キャストに成功したら
				if (lockOnCamera)
				{
					lockOnCamera->SetTargetEnemy(closestEnemy);
					lockOnCamera->SetPlayer(pPlayer_);

					//カメラをロックオンカメラに切り替える
					pCameraManager_->ChangeCamera(L"LockOnCamera");

					//ロックオン開始
					pPlayer_->SetLockOn(true);
					pPlayer_->SetLockOnEnemy(closestEnemy);
				}
			}
		}
	}

	auto playerCam = std::dynamic_pointer_cast<PlayerCamera>(pCameraManager_->GetActiveCamera());
	if (playerCam)
	{
		Vector3 stickR = Input::GetInstance().GetStickRight();
		playerCam->AddRotation(-stickR.x_ * 0.03f, -stickR.z_ * 0.03f);
	}

	//すべてのゲームオブジェクトの更新
	for (auto& obj : gameObjects_)
	{

		if (!obj->IsDead())
		{
			obj->Update();
		}
	}

	//敵マネージャーの更新
	for (auto& enemy : pEnemyManager_->GetEnemies())
	{
		enemy->Update();
	}

	//カメラマネージャーの更新
	pCameraManager_->Update(pStage_->GetHandle());

	//当たり判定の更新
	CollisionManager::GetInstance().UpdateCheckCollision();

	//死んでいるゲームオブジェクトの削除
	gameObjects_.erase(
		std::remove_if(gameObjects_.begin(), gameObjects_.end(), [](const auto& obj) {
			return obj->IsDead();
			}),
		gameObjects_.end()
	);

	//死んでいる敵の削除
	pEnemyManager_->RemoveEnemy();
}

void GameScene::FadeOutUpdate()
{
	if (frameCount_-- <= 0)
	{
		sceneManager_.ChangeScene(std::make_shared<ResultScene>(sceneManager_));
	}
}

void GameScene::FadeDraw()
{
	NormalDraw();

	float rate;

	if (update_ == &GameScene::FadeInUpdate)
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

void GameScene::NormalDraw()
{
	//すべてのオブジェクトの描画
	for (auto& obj : gameObjects_)
	{
		if (!obj->IsDead())
		{
			obj->Draw();
		}
	}

	//敵マネージャーの描画
	pEnemyManager_->Draw();

	DrawGrid();

#ifdef _DEBUG
	DrawString(0, 0, L"GameScene", GetColor(255, 255, 255));
	DrawFormatString(0, 30, GetColor(255, 255, 255), L"FRAME:%d", frameCount_);
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
