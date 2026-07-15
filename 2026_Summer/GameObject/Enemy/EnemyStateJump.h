#pragma once
#include "EnemyStateBase.h"
#include "Math/Vector3.h"

class EnemyStateJump :public EnemyStateBase
{
public:
	EnemyStateJump(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateJump() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;
private:
	float deltaTime_;	//経過時間

	Vector3 jumpStartPos_;	//ジャンプのスタート位置
	Vector3 jumpEndPos_;	//ジャンプの終了位置
};

