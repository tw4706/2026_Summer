#pragma once
#include<memory>
#include <vector>

class Oni;
class Player;
class Input;
class Stage;
class GameObject;
class CameraManager;
class CollisionManager;
class GameScene
{
public:
	GameScene();
	~GameScene();

	void Init(Input&input);
	void Update(Input& input);
	void Draw();

	/// <summary>
	/// ゲームオブジェクトの登録
	/// </summary>
	/// <param name="obj">登録するオブジェクトの名前</param>
	void RegisterGameObject(std::shared_ptr<GameObject>obj);

private:
	void DrawGrid();

private:
	//フレームカウンタ
	int frameCount_;

	//ステージ
	std::shared_ptr<Stage>pStage_;

	//プレイヤー
	std::shared_ptr<Player>pPlayer_;

	//鬼
	std::shared_ptr<Oni> pOni_;

	//カメラ(シーンが所有しているためunique_ptrで持つ)
	std::unique_ptr<CameraManager>pCameraManager_;

	//コリジョンマネージャー
	std::unique_ptr<CollisionManager>pCollisionManager_;

	//ゲームオブジェクトを管理する用のリスト
	std::vector<std::shared_ptr<GameObject>>gameObjects_;

	//ゲームオブジェクト予約用のリスト
	std::vector<std::shared_ptr<GameObject>>reserveObjList_;
};

