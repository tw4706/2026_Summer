#pragma once

class SceneManager;
class Scene
{
public:
	Scene(SceneManager& scene);

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init()abstract;

	/// <summary>
	/// シーンの更新
	/// </summary>
	virtual void Update()abstract;

	/// <summary>
	/// シーンの描画
	/// </summary>
	virtual void Draw()abstract;

protected:
	//シーンのマネージャーの参照
	SceneManager& sceneManager_;
};

