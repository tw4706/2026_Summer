#pragma once
#include "Collider.h"
class PolygonCollider :public Collider
{
public:
	PolygonCollider(int modelHandle);
	virtual~PolygonCollider()override;

	void Update()override;

	int GetModelHandle()const { return modelHandle_; }

private:
	int modelHandle_;	//ƒ‚ƒfƒ‹ƒnƒ“ƒhƒ‹
};

