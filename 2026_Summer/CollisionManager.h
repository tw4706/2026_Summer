#pragma once
#include<vector>
#include<memory>

class Stage;
class Vector3;
class Collider;
class Collidable;
class CollisionManager
{
public:

	/// <summary>
	/// インスタンスの作成
	/// </summary>
	/// <returns></returns>
	static CollisionManager& GetInstance();

	CollisionManager();
	virtual~CollisionManager();

	//コピー禁止
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;

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
	bool CheckSphereVsCapsule(Collidable& sphereObj, Collidable& capsuleObj);

	/// <summary>
	/// 2つの球体コライダーの当たり判定
	/// </summary>
	bool CheckSphereVsSphere(Collidable& sphereObj, Collidable& capsuleObj);

	/// <summary>
	/// 2つのカプセルコライダーの当たり判定
	/// </summary>
	bool CheckCapsuleVsCapsule(Collidable& pCapsuleObjA, Collidable& pCapsuleObjB);

	/// <summary>
	/// カプセルとポリゴンの当たり判定
	/// </summary>
	bool CheckCapsuleVsPolygon(Collidable& pCapsuleObj, Collidable& pPolygonObj);
private:
	// ゲーム内に存在する全コライダーのポインタ配列
	std::vector<Collider*> pAllColliders_;
};

