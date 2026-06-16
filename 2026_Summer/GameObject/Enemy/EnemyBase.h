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

	void ChangeState(std::shared_ptr<EnemyStateBase>nextState);

	void AddPosition(const Vector3& offset);

	Vector3 GetVelocity() const { return vel_; }
	void SetVelocity(const Vector3& vel) { vel_ = vel; }
};

