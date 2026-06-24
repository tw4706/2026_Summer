#pragma once
#include<vector>

class Stage;
class Collider;
class Character;
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
	/// <param name="pSphere">球のコライダーのポインタ</param>
	/// <param name="pCapsule">カプセルコライダーのポインタ</param>
	void CheckSphereVsCapsule(Collider* pSphere, Collider* pCapsule);

	/// <summary>
	/// 2つの球体コライダーの当たり判定
	/// </summary>
	/// <param name="pSphereA">最初の球体コライダーのポインタ。</param>
	/// <param name="pSphereB">2個目の球体コライダーのポインタ。</param>
	void CheckSphereVsSphere(Collider* pSphereA, Collider* pSphereB);

	/// <summary>
	/// 2角カプセルコライダーの当たり判定
	/// </summary>
	/// <param name="pCapsuleA">最初のカプセルコライダーのポインタ</param>
	/// <param name="pCapsuleB">2つめのカプセルコライダーのポインタ</param>
	void CheckCapsuleVsCapsule(Collider* pCapsuleA, Collider* pCapsuleB);
private:
	// ゲーム内に存在する全コライダーのポインタ配列
	std::vector<Collider*> pAllColliders_;
};

