#pragma once
#include "Scene.h"
#include<memory>
#include <vector>

class Player;
class Stage;
class GameObject;
class EnemyManager;
class CameraManager;
class CollisionManager;
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

	/// <summary>
	/// グリッド線の描画
	/// </summary>
	void DrawGrid();
private:
	//フレームカウンタ
	int frameCount_;

	//ステージ
	std::shared_ptr<Stage>pStage_;

	//プレイヤー
	std::shared_ptr<Player>pPlayer_;

	//カメラ(シーンが所有しているためunique_ptrで持つ)
	std::unique_ptr<CameraManager>pCameraManager_;

	//エネミーマネージャー
	std::unique_ptr<EnemyManager>pEnemyManager_;

	//ゲームオブジェクトを管理する用のリスト
	std::vector<std::shared_ptr<GameObject>>gameObjects_;

	//ゲームオブジェクト予約用のリスト
	std::vector<std::shared_ptr<GameObject>>reserveObjList_;
};

