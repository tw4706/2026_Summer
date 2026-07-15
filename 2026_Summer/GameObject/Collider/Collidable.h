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
	/// コライダーを生成して登録を行うテンプレート
	/// </summary>
	/// <typeparam name="T">Colliderの派生クラス</typeparam>
	/// <param name="args">Tのコンストラクタ引数</param>
	/// <returns>生成したコライダーへのポインタ(所有権はCollidableのまま)</returns>
	template<typename T, typename... Args>
	T* CreateCollider(Args&&... args)
	{
		auto pCollider = std::make_unique<T>(std::forward<Args>(args)...);
		T* pRawPtr = pCollider.get();
		AddCollider(std::move(pCollider));
		return pRawPtr;
	}

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="obj">ゲームオブジェクト</param>
	virtual void OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)abstract;

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
	/// 地面との設置判定フラグの取得
	/// </summary>
	/// <returns>地面と接地しているかどうか</returns>
	bool GetIsGround() const { return isGround_; }

	/// <summary>
	/// 地面との設置判定の設定
	/// </summary>
	/// <param name="isGround">地面に触れているかどうかのフラグ</param>
	void SetIsGround(bool isGround) { isGround_ = isGround; }

	/// <summary>
	/// コライダーの取得
	/// </summary>
	/// <returns></returns>
	const std::vector<std::unique_ptr<Collider>>& GetColliders()const;

protected:
	bool isHit_ = false;						//当たっているかどうか
	bool isEnabled_ = false;					//有効かどうか
	bool isGround_ = false;						//地面についているかどうか

	//コライダーの配列
	std::vector<std::unique_ptr<Collider>>colliders_;
};

