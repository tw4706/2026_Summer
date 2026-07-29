#pragma once
#include "Scene.h"
#include "Math/Vector3.h"
#include<memory>
#include <vector>

class Bg;
class Player;
class Stage;
class GameObject;
class EnemyManager;
class CameraManager;
class CollisionManager;
class LockOnManager;
class UIManager;
class GameScene :public Scene
{
public:
	GameScene(SceneManager& sceneManager);
	~GameScene();

	void Init()override;
	void Update();
	void Draw()override;

	void FadeInUpdate();
	void NormalUpdate();
	void FadeOutUpdate();
	using UpdateFunc_t = void (GameScene::*)();
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (GameScene::*)();
	DrawFunc_t draw_;

	/// <summary>
	/// ゲームオブジェクトの登録
	/// </summary>
	/// <param name="obj">登録するオブジェクトのスマートポインタ</param>
	void RegisterGameObject(std::shared_ptr<GameObject>obj);

private:
	//ボストリガーの判定
	void CheckBossTrigger();

	//ボスの生成
	void SpawnBoss();
private:
	//フレームカウンタ
	int frameCount_;

	//ボス出現をトリガーするZ座標
	float bossTriggerPosZ_;

	//トリガー判定半径
	float bossTriggerRadius_;

	//ボスが出現済みかどうか
	bool isBossSpawned_;

	//UIマネージャー
	std::shared_ptr<UIManager> pUiManager_;

	//ステージ
	std::shared_ptr<Stage>pStage_;

	//背景
	std::shared_ptr<Bg>pBg_;

	//プレイヤー
	std::shared_ptr<Player>pPlayer_;

	//カメラ(シーンが所有しているためunique_ptrで持つ)
	std::unique_ptr<CameraManager>pCameraManager_;

	//エネミーマネージャー
	std::unique_ptr<EnemyManager>pEnemyManager_;

	//ロックオンマネージャー
	std::unique_ptr<LockOnManager>pLockOnManager_;

	//ゲームオブジェクトを管理する用のリスト
	std::vector<std::shared_ptr<GameObject>>gameObjects_;

	//ゲームオブジェクト予約用のリスト
	std::vector<std::shared_ptr<GameObject>>reserveObjList_;
};

