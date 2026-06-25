#pragma once
#include<vector>

class Stage;
class Vector3;
class Collider;
class Collidable;
class CollisionManager
{
public:
	CollisionManager();
	virtual~CollisionManager();

	/// <summary>
	/// コライダーの登録
	/// </summary>
	/// <param name="pCollider">コライダーのポインタ</param>
	void RegisterCollider(Collider* pCollider);

	/// <summary>
	/// コライダーの登録解除
	/// </summary>
	/// <param name="pCollider">解除するコライダーのポインタ</param>
	void UnRegisterCollider(Collider* pCollider);

	/// <summary>
	/// 衝突判定の更新
	/// </summary>
	/// <param name="pStage">ステージのポインタ</param>
	void UpdateCheckCollision(Stage* pStage);

private:
	/// <summary>
	/// 球とカプセルの当たり判定
	/// </summary>
	bool CheckSphereVsCapsule(Collidable* pSphereObj, Collidable* pCapsuleObj);

	/// <summary>
	/// 2つの球体コライダーの当たり判定
	/// </summary>
	bool CheckSphereVsSphere(Collidable* pSphereObjA, Collidable* pSphereObjB);

	/// <summary>
	/// 2つのカプセルコライダーの当たり判定
	/// </summary>
	bool CheckCapsuleVsCapsule(Collidable* pCapsuleObjA, Collidable* pCapsuleObjB);
private:
	// ゲーム内に存在する全コライダーのポインタ配列
	std::vector<Collider*> pAllColliders_;
};

