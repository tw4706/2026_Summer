#pragma once
#include "CameraBase.h"

class Player;
class EnemyBase;
class LockOnCamera :public CameraBase
{
public:
	LockOnCamera();
	virtual~LockOnCamera();

	virtual void Init()override;
	virtual void Update(int stageModelHandle)override;

	void SetPlayer(std::weak_ptr<Player>pPlayer) { pPlayer_ = pPlayer; }
	void SetTargetEnemy(std::weak_ptr<EnemyBase>pEnemy) { pEnemy_ = pEnemy; }

private:
	Vector3 lastDirectionVec_;

	std::weak_ptr<Player>pPlayer_;//プレイヤーのポインタ
	std::weak_ptr<EnemyBase>pEnemy_;//敵のポインタ
};

