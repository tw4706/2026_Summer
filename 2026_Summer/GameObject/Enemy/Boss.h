#pragma once
#include "EnemyBase.h"
class Boss :public EnemyBase
{
public:
	Boss();
	virtual~Boss();

	virtual void Init()override;
	virtual void Update()override;
};

