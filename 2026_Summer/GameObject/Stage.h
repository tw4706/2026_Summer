#pragma once
#include "Collider/Collidable.h"
#include "Model.h"
#include "NavigationGrid.h"

class Stage :public Collidable
{
public:
	Stage();
	Stage(Vector3 pos, Vector3 vel,float angle);
	~Stage();

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="coll"></param>
	void OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)override {}

	/// <summary>
	/// ハンドルの取得
	/// </summary>
	/// <returns>ハンドルを返す</returns>
	int GetHandle()const { return stageColModel_.GetHandle(); }

	/// <summary>
	/// ナビゲーショングリッドの取得
	/// </summary>
	/// <returns>NavigationGridのポインタ</returns>
	const NavigationGrid* GetNaviGrid() const { return &navGrid_; }

private:
	/// <summary>
	/// デバッグ用のナビゲーショングリッドの描画
	/// </summary>
	void DrawNavGridDebug() const;

private:
	Model stageColModel_;	//当たり判定用のモデル
	Model stageModel_;		//見た目用のモデル
	NavigationGrid navGrid_;
};

