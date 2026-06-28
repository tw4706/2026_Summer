#pragma once
#include "Collider/Collidable.h"
#include "Model.h"
#include "NavigationGrid.h"

class Stage :public Collidable
{
public:
	Stage(Vector3 pos, Vector3 vel, float dir);
	~Stage();

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="coll"></param>
	void OnCollision(Collidable& coll)override {}

	/// <summary>
	/// ハンドルの取得
	/// </summary>
	/// <returns>ハンドルを返す</returns>
	int GetHandle()const { return stageModel_.GetHandle(); }



private:
	/// <summary>
	/// ナビゲーショングリッドの取得
	/// </summary>
	/// <returns>NavigationGridの参照</returns>
	const NavigationGrid& GetNavGrid() const { return navGrid_; }

	/// <summary>
	/// デバッグ用のナビゲーショングリッドの描画
	/// </summary>
	void DrawNavGridDebug() const;

private:
	Model stageModel_;
	NavigationGrid navGrid_;
};

