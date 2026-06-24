#pragma once
#include "GameObject.h"
#include<vector>
#include <memory>

class Collider;
class Collidable :public GameObject
{
public:
	Collidable(Vector3 pos, Vector3 vel, float dir);
	virtual~Collidable();

	void Update();

	/// <summary>
	/// 当たっているかどうか
	/// </summary>
	/// <returns>オブジェクトと当たっているかどうかを返す</returns>
	bool IsHit()const { return isHit_; }

	/// <summary>
	/// 当たり判定の衝突更新を行うためのヒットフラグのリセット
	/// </summary>
	void ResetHitFlag() { isHit_ = false; }

	/// <summary>
	/// コライダーの登録
	/// </summary>
	/// <param name="pCollider">コライダーのポインタ</param>
	void AddCollider(std::unique_ptr<Collider> pCollider);

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="obj">ゲームオブジェクト</param>
	virtual void OnCollision(GameObject* obj)abstract;

	/// <summary>
	/// 当たり判定の有効/無効を設定する
	/// </summary>
	/// <param name="isEnabled">trueなら当たり判定有効</param>
	void SetEnabled(bool isEnabled) { isEnabled_ = isEnabled; }
	/// <summary>
	/// 当たり判定が有効かどうかを取得する
	/// </summary>
	bool IsEnabled() const { return isEnabled_; }

	/// <summary>
	/// コライダーの取得
	/// </summary>
	/// <returns></returns>
	const std::vector<std::unique_ptr<Collider>>& GetColliders()const;

protected:
	bool isHit_ = false;						//当たっているかどうか
	bool isEnabled_ = false;					//有効かどうか

	//コライダーの配列
	std::vector<std::unique_ptr<Collider>>colliders_;
};

