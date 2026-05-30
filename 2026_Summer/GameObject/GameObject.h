#pragma once
#include"Vector3.h"

class Collider;
class GameObject
{
public :
	GameObject(Vector3 pos,Vector3 vel,float dir);
	virtual~GameObject();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標の参照</returns>
	const Vector3& GetPos() const { return pos_; }

protected:
	Vector3 pos_;//座標
	Vector3 vel_;//速度
	float dir_;//向き
};

