#pragma once
#include "EnemyStateBase.h"
#include "Math/Vector3.h"

class EnemyStateReact :public EnemyStateBase
{
public:
	EnemyStateReact(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateReact() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
private:
	Vector3 toPlayerDir_ = {0.0f,0.0f,0.0f};//プレイヤーに向く角度
	float reactTimer_ = 0.0f;//反応する時間をカウントするタイマー
};

