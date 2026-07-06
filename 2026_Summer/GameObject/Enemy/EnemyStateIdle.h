#pragma once
#include "EnemyStateBase.h"

class WayPointLoader;
class EnemyStateIdle :public EnemyStateBase
{
public:
	EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateIdle() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
private:
	//WayPointLoader‚Ìƒ|ƒCƒ“ƒ^
	std::shared_ptr<WayPointLoader>pLoader_;
};

