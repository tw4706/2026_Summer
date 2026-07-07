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

private:
	std::weak_ptr<Player>pPlayer_;//プレイヤーのポインタ
	std::weak_ptr<EnemyBase>pEnemy_;//敵のポインタ
};

