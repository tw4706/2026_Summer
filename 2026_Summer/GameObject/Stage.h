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

	/// <summary>
	/// ナビゲーショングリッドの取得
	/// </summary>
	/// <returns>NavigationGridのポインタ</returns>
	const NavigationGrid* GetNaviGrid() const { return &navGrid_; }

private:

	/// <summary>
	/// モデルの頂点情報からステージの範囲(XZ平面の最大最小)を計算する
	/// </summary>
	void CalculateStageBounds(int modelHandle, float& outMinX, float& outMaxX, float& outMinZ, float& outMaxZ);

	/// <summary>
	/// デバッグ用のナビゲーショングリッドの描画
	/// </summary>
	void DrawNavGridDebug() const;

private:
	Model stageModel_;
	NavigationGrid navGrid_;
};

