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

private:
	int modelH_;//モデルハンドル
	float moveAngle_;//向いている角度
};

