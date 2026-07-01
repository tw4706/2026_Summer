#pragma once
#include "EnemyStateBase.h"
#include"Math/Vector3.h"

class EnemyStateReturn :public EnemyStateBase
{
public:
	EnemyStateReturn(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateReturn() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;

private:
	Vector3 returnNextPos_ = { 0.0f,0.0f,0.0f };

	int fromWayPointId_ = -1;
	int toWayPointId_ = -1;
};

