#pragma once
#include<memory>
#include<list>

class Scene;
class SceneManager
{
public:

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns>SceneManagerのインスタンス</returns>
	static SceneManager& GetInstance();

	SceneManager();
	virtual~SceneManager();

	//コピーを禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	/// <summary>
	/// シーンのリセット
	/// </summary>
	/// <param name="scene">リセットするシーン名</param>
	void ResetScene(std::shared_ptr<Scene>scene);

	/// <summary>
	/// シーンの切り替え
	/// </summary>
	/// <param name="scene">切り替えるシーン名</param>
	void ChangeScene(std::shared_ptr<Scene>scene);

	/// <summary>
	/// シーンを積む
	/// </summary>
	/// <param name="scene">積むシーン名</param>
	void PushScene(std::shared_ptr<Scene>scene);

	/// <summary>
	/// 最後に積んだシーンを外す
	/// </summary>
	void PopScene();

	/// <summary>
	/// シーンの描画
	/// </summary>
	void Update();

	/// <summary>
	/// シーンの描画
	/// </summary>
	void Draw();

private:
	//シーンのリスト
	std::list<std::shared_ptr<Scene>>scenes_;

};

