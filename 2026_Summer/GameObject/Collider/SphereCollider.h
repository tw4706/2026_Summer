#pragma once
#include "Collider.h"
class SphereCollider :public Collider
{
public:
	SphereCollider(float r);
	virtual~SphereCollider()override;

	float GetRadius()const { return r_; }

	void SetRadian(float r) { r_ = r; }
private:
	float r_;	//”¼Œa
};

