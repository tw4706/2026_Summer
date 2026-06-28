#pragma once
#include "EnemyBase.h"

class Oni :public EnemyBase
{
public:
	Oni();
	virtual~Oni();

	void Init()override;
	void Draw()override;

	//void OnCollision(Collidable& coll)override;
};

