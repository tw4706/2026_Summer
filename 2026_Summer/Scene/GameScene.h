#pragma once
#include<memory>
#include <vector>

class Player;
class Camera;
class Input;
class GameObject;
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

	//プレイヤー
	std::shared_ptr<Player>pPlayer_;

	//カメラ
	std::shared_ptr<Camera>pCamera_;

	//ゲームオブジェクトを管理する用のリスト
	std::vector<std::shared_ptr<GameObject>>gameObjects_;

	//ゲームオブジェクト予約用のリスト
	std::vector<std::shared_ptr<GameObject>>reserveObjList_;
};

