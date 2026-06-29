#pragma once
#include "EnemyBase.h"
class BigMan :public EnemyBase
{
public:
	BigMan();
	virtual~BigMan()=default;

	void Init()override;
	void Draw()override;
};

