#pragma once
#include "Collider.h"
#include"Math/Vector3.h"

class SphereCollider :public Collider
{
public:
	SphereCollider(float r);
	virtual~SphereCollider()override;

	void Update()override {};

	float GetRadius()const { return r_; }

	void SetRadian(float r) { r_ = r; }

	const Vector3& GetPos()const { return pos_; }

	void SetPos(const Vector3& pos) { pos_ = pos; }
private:
	Vector3 pos_;
	float r_;	//”¼Œa
};

