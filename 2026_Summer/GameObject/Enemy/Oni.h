#pragma once
#include "EnemyBase.h"

class Oni :public EnemyBase
{
public:
	Oni();
	virtual~Oni();

	void Init()override;
	void Update()override;
	void Draw()override;

	void OnCollision(GameObject* obj)override;
};

