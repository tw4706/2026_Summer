#pragma once
#include "GameObject.h"
#include<vector>
class Collider;
class Collidable :public GameObject
{
public:
	Collidable(Vector3 pos, Vector3 vel, float dir);
	virtual~Collidable();

	/// <summary>
	/// コライダーの登録
	/// </summary>
	/// <param name="pCollider">コライダーのポインタ</param>
	void AddCollider(Collider* pCollider);

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="obj">ゲームオブジェクト</param>
	virtual void OnCollision(GameObject*obj)abstract;

	/// <summary>
	/// コライダーの取得
	/// </summary>
	/// <returns></returns>
	const std::vector<Collider*>& GetColliders()const;

protected:
	//コライダーの配列
	std::vector<Collider*>colliders_;
};

