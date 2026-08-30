#include "GameScene.h"
#include"Bg.h"
#include"Stage.h"
#include"Input.h"
#include"Katana.h"
#include"Enemy/Oni.h"
#include"GameObject.h"
#include"EnemyManager.h"
#include"SceneManager.h"
#include"FadeManager.h"
#include"ResultScene.h"
#include"Enemy/BigMan.h"
#include"Enemy/Boss/Boss.h"
#include"Player/Player.h"
#include"CollisionManager.h"
#include"Camera/PlayerCamera.h"
#include"Camera/BossCamera.h"
#include"Camera/LockOnCamera.h"
#include"Camera/CameraManager.h"
#include"LockOnManager.h"
#include"SoundManager.h"
#include"UIManager.h"
#include"UI/EnemyHPGaugeUI.h"
#include"UI/PlayerHPGaugeUI.h"
#include"UI/BossHPGaugeUI.h"
#include"UI/ReticleUI.h"
#include"Game.h"
#include"EffekseerForDXLib.h"
#include <memory>
#include <DxLib.h>
#include <algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//最大敵エリアのスポーン数
	constexpr int kMaxSpawnArea= 3;

	//ボス用のエリアID
	constexpr int kBossAreaId = 3;

	//デバッグワープ後の座標
	const Vector3 kDebugWarpPlayerPos = { -4100.0f,0.0f,-10000.0f };

	//フレームレート
	constexpr int kFps = 60;
}

GameScene::GameScene(SceneManager& sceneManager) :
	Scene(sceneManager),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw),
	frameCount_(kFadeInterval),
	bossTriggerPosZ_( -10250.0f),
	bossTriggerRadius_(1500.0f),
	isBossSpawned_(false)
{
	pBg_ = std::make_shared<Bg>();
	pPlayer_ = std::make_shared<Player>();
	pCameraManager_ = std::make_unique<CameraManager>();
	pEnemyManager_ = std::make_unique<EnemyManager>();
	pLockOnManager_ = std::make_unique<LockOnManager>();
	pUiManager_ = std::make_shared<UIManager>();

	//ゲームオブジェクトの登録
	//カメラの登録
	auto playerCamera = std::make_shared<PlayerCamera>();
	pCameraManager_->RegisterCamera(L"PlayerCamera", playerCamera);
	auto lockOnCamera = std::make_shared<LockOnCamera>();
	pCameraManager_->RegisterCamera(L"LockOnCamera", lockOnCamera);
	auto bossCamera = std::make_shared<BossCamera>();
	pCameraManager_->RegisterCamera(L"BossCamera", bossCamera);

	//ステージの登録
	pStage_ = std::make_shared<Stage>(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f);
	RegisterGameObject(pStage_);

	//CSV読み込み
	pEnemyManager_->LoadEnemyData(L"data/CSV/EnemyData.csv");
	pEnemyManager_->LoadEnemySpawnData(L"data/CSV/EnemySpawnData.csv");
	pEnemyManager_->LoadWayPointData(L"data/CSV/WayPointData.csv");
	pEnemyManager_->LoadEnemyAttackData(L"data/CSV/EnemyAttackData.csv");

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
	frameCount_ = kFadeInterval;

	//カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(true);

	//背景の初期化
	pBg_->Init(L"data/Bg/backGround");

	//現在のアクティブカメラを取得
	auto activeCam = pCameraManager_->GetActiveCamera();

	//プレイヤーカメラへの変換
	auto playerCam = std::dynamic_pointer_cast<PlayerCamera>(activeCam);

	//ゲームオブジェクトのリストがある場合
	if (!reserveObjList_.empty())
	{
		//各オブジェクトについているコライダーの登録
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

		//優先度順に処理する
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
			player->SetCameraManager(pCameraManager_.get());
			player->Init();

			auto playerGauge = std::make_shared<PlayerHPGaugeUI>(player);
			pUiManager_->AddUI(playerGauge);

			auto reticleUI = std::make_shared<ReticleUI>(player);
			pUiManager_->AddUI(reticleUI);
		}
	}

	//プレイヤーカメラの初期化
	if (playerCam)
	{
		playerCam->SetPlayer(pPlayer_);
		playerCam->Init();
	}

	//敵の生成
	for (int areaId = 0; areaId < kMaxSpawnArea; ++areaId)
	{
		auto enemies = pEnemyManager_->SpawnEnemyArea(areaId);

		for (auto& enemy : enemies)
		{
			enemy->SetPlayer(pPlayer_);
			enemy->SetNavigationGrid(pStage_->GetNaviGrid());
			enemy->SetStageModelHandle(pStage_->GetHandle());

			auto enemyGauge = std::make_shared<EnemyHPGaugeUI>(enemy);
			pUiManager_->AddUI(enemyGauge);
			enemy->SetHPGaugeUI(enemyGauge);
		}
	}

	//カメラマネージャーの更新
	pCameraManager_->Update(pStage_->GetHandle());

	SoundManager::GetInstance().PlayBgm(BGM::Game);
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

	//UIマネージャーの更新
	pUiManager_->Update();

	frameCount_--;

	if (frameCount_ <= 0)
	{
		update_ = &GameScene::NormalUpdate;
		draw_ = &GameScene::NormalDraw;
	}
}

void GameScene::NormalUpdate()
{
	frameCount_++;
	clearFrameCount_++;

#ifdef _DEBUG
	//ワープ(デバッグのみ)
	if (Input::GetInstance().IsTriggered("debugWarp"))
	{
		pPlayer_->SetPos(kDebugWarpPlayerPos);
	}
	if (Input::GetInstance().IsTriggered("debugKillAllEnemies"))
	{
		pEnemyManager_->AllEnemiesKill();
	}
#endif

	//プレイヤーが死んだ場合はシーン遷移をする
	if (pPlayer_->IsDead())
	{
		isGameOver_ = true;
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frameCount_ = kFadeInterval;
		return;
	}

	//ボストリガー判定
	CheckBossTrigger();

	if (isBossSpawned_ && !pPlayer_->GetCanControl())
	{
		auto bossCam = std::dynamic_pointer_cast<BossCamera>(pCameraManager_->GetCamera(L"BossCamera"));
		if (bossCam && bossCam->IsBossEventFinished())
		{
			pPlayer_->SetCanControl(true);
		}
	}

	//ボスが生成されるまでは生成範囲まででプレイヤーを制限
	if (!isBossSpawned_)
	{
		auto pos = pPlayer_->GetPos();

		if (pos.z_ <= bossTriggerPosZ_)
		{
			pos.z_ = bossTriggerPosZ_;
			pPlayer_->SetPos(pos);
		}
	}

	//ロックオンボタンを押したらロックオンを開始する
	if (Input::GetInstance().IsTriggered("lockOn"))
	{
		pLockOnManager_->StartLockOn(pPlayer_, pEnemyManager_->GetEnemies(), pCameraManager_.get());
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

	//ロックオンマネージャーの更新
	pLockOnManager_->Update(pPlayer_, pEnemyManager_->GetEnemies(), pCameraManager_.get());

	//UIマネージャーの更新
	pUiManager_->Update();

	Effekseer_Sync3DSetting();

	//ボスを倒したらリザルトに遷移
	auto boss = pBoss_.lock();
	if (boss && boss->IsDead())
	{
		isGameOver_ = false;
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frameCount_ = kFadeInterval;
		clearTime_ = static_cast<float>(clearFrameCount_) / kFps;
		return;
	}

	//死んでいるゲームオブジェクトの削除
	gameObjects_.erase(std::remove_if(gameObjects_.begin(), gameObjects_.end(), 
		[](const auto& obj) {return obj->IsDead();}),gameObjects_.end());

	//死んでいる敵の削除
	pEnemyManager_->RemoveEnemy();
}

void GameScene::FadeOutUpdate()
{
	frameCount_--;

	//時間が経過したらリザルトシーンに遷移
	if (frameCount_ <= 0)
	{
		sceneManager_.ChangeScene(std::make_shared<ResultScene>(sceneManager_, clearTime_,isGameOver_));
	}
}

void GameScene::BossFadeOutUpdate()
{
	frameCount_--;

	if (frameCount_ <= 0)
	{
		SpawnBoss();
		update_ = &GameScene::FadeInUpdate;
		frameCount_ = kFadeInterval;
	}
}

void GameScene::FadeDraw()
{
	NormalDraw();

	float rate;

	if (update_ == &GameScene::FadeInUpdate)
	{
		//フェードイン
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//フェードアウト
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}
	rate = std::clamp(rate, 0.0f, 1.0f);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::NormalDraw()
{
	pBg_->Draw(pCameraManager_->GetActiveCamera()->GetPos());

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

	//UIマネージャーの描画
	pUiManager_->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"GameScene",Game::kWhiteColor);
	DrawFormatString(0, 30, Game::kWhiteColor, L"FRAME:%d", frameCount_);
#endif
}

void GameScene::RegisterGameObject(std::shared_ptr<GameObject> obj)
{
	//予約リストへの追加
	reserveObjList_.push_back(obj);
}

void GameScene::CheckBossTrigger()
{
	//既に出現しているなら何もしない
	if (isBossSpawned_)return;

	//雑魚敵をいないかつプレイヤーがボスの生成する範囲に入っていたらボスを生成
	if (pEnemyManager_->GetEnemies().empty()&&pPlayer_->GetPos().z_ <= bossTriggerPosZ_)
	{
		isBossSpawned_ = true;
		update_ = &GameScene::BossFadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frameCount_ = kFadeInterval;
	}
}

void GameScene::SpawnBoss()
{
	//ボス専用エリアIDのみ生成
	auto enemies = pEnemyManager_->SpawnEnemyArea(kBossAreaId);

	for (auto& enemy : enemies)
	{
		enemy->SetPlayer(pPlayer_);
		enemy->SetNavigationGrid(pStage_->GetNaviGrid());
		enemy->SetStageModelHandle(pStage_->GetHandle());

		//ボスならBossHPGaugeUI、それ以外はEnemyHPGaugeUIを生成
		std::shared_ptr<EnemyHPGaugeUI> enemyGauge;
		if (auto boss = std::dynamic_pointer_cast<Boss>(enemy))
		{
			enemyGauge = std::make_shared<BossHPGaugeUI>(boss);
		}
		else
		{
			enemyGauge = std::make_shared<EnemyHPGaugeUI>(enemy);
		}

		pUiManager_->AddUI(enemyGauge);
		enemy->SetHPGaugeUI(enemyGauge);

		//ボスなら弱参照として保持
		if (auto boss = std::dynamic_pointer_cast<Boss>(enemy))
		{
			pBoss_ = boss;

			//ボスカメラにターゲットを設定して切り替え
			auto bossCam = std::dynamic_pointer_cast<BossCamera>(pCameraManager_->GetCamera(L"BossCamera"));
			if (bossCam)
			{
				bossCam->SetTarget(pPlayer_, boss,pCameraManager_.get());
			}
			pCameraManager_->ChangeCamera(L"BossCamera");

			//ボス用BGMに切り替え
			SoundManager::GetInstance().PlayBgm(BGM::BossBattle);
		}
	}

	//プレイヤーの操作を不可能にする
	pPlayer_->SetCanControl(false);
};
