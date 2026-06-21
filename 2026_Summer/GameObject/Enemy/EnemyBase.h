#pragma once
#include "Character.h"
#include "Animation.h"

class EnemyStateBase;
class EnemyBase :public Character
{
public:
	EnemyBase();
	virtual~EnemyBase();

	virtual void Init()override {};
	virtual void Update()override {};
	virtual void Draw()override {};
};

