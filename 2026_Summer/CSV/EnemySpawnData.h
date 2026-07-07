#pragma once
#include"Math/Vector3.h"
#include<string>

struct EnemySpawnData
{
	//ƒGƒŠƒAID
	int areaId_;

	//“G‚Ìí—Ş
	std::string type_;

	//HP
	int hp_ = 0;

	//¶¬À•W
	Vector3 spawnPos_;

	//‰ñ“]Šp“x
	float rotateX = 0.0f;
	float rotateY = 0.0f;
	float rotateZ = 0.0f;

	//Šg‘å—¦
	Vector3 scale_;
};