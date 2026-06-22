#pragma once

enum class ColliderType
{
	Sphere,
	Capsule,
};

class Collidable;
class Collider
{
public:
	Collider(ColliderType type);
	virtual~Collider() {}

	virtual void Update()abstract;

	/// <summary>
	/// コライダーの種類の取得
	/// </summary>
	/// <returns>コライダーの種類</returns>
	ColliderType GetType()const { return type_; }

	/// <summary>
	/// コライダーのオーナーのセット
	/// </summary>
	/// <param name="owner">コライダーを持つオブジェクト</param>
	void SetOwner(Collidable* owner);

	Collidable* GetOwner()const { return pOwner_; }

protected:
	ColliderType type_;		//コライダーの種類
	Collidable* pOwner_;	//コライダーの持つオブジェクトの参照
};

