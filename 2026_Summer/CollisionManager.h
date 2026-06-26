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
	void UpdateCheckCollision();

private:
	/// <summary>
	/// 球とカプセルの当たり判定
	/// </summary>
	bool CheckSphereVsCapsule(const std::shared_ptr<Collidable> pSphereObj, const std::shared_ptr<Collidable> pCapsuleObj);

	/// <summary>
	/// 2つの球体コライダーの当たり判定
	/// </summary>
	bool CheckSphereVsSphere(const std::shared_ptr<Collidable> pSphereObjA, const std::shared_ptr<Collidable> pSphereObjB);

	/// <summary>
	/// 2つのカプセルコライダーの当たり判定
	/// </summary>
	bool CheckCapsuleVsCapsule(const std::shared_ptr<Collidable> pCapsuleObjA, const std::shared_ptr<Collidable> pCapsuleObjB);

	/// <summary>
	/// カプセルとポリゴンの当たり判定
	/// </summary>
	bool CheckCapsuleVsPolygon(const std::shared_ptr<Collidable> pCapsuleObj, Collidable* pPolygonObj);
private:
	// ゲーム内に存在する全コライダーのポインタ配列
	std::vector<Collider*> pAllColliders_;
};

