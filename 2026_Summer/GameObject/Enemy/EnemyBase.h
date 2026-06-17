#pragma once
#include "Character.h"
#include "Animation.h"

class EnemyStateBase;
class EnemyBase :public Character
{
public:
	EnemyBase();
	virtual~EnemyBase();

	void Init()override;
	void Update()override;
	void Draw()override;
};

