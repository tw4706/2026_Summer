#pragma once

class SceneManager;
class Scene
{
public:
	Scene(SceneManager& scene);

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// シーンの更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// シーンの描画
	/// </summary>
	virtual void Draw() = 0;

protected:
	//シーンのマネージャーの参照
	SceneManager& sceneManager_;
};

