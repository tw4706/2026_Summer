#pragma once
#include "EnemyStateBase.h"
class EnemyStateReact :public EnemyStateBase
{
public:
	EnemyStateReact(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateReact() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
private:
	float moveAngleFrame_=0.0f;//‰ñ“]Šp“x‚ÌƒtƒŒ[ƒ€
	float baseAngle_ = 0.0f;//Œ³‚Æ‚È‚éŠp“x
};

