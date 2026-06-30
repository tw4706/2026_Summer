#include "EnemyStateBase.h"
#include "Enemy/EnemyBase.h"
#include "Vector3.h"
#include "Camera/CameraBase.h"

EnemyStateBase::EnemyStateBase(std::weak_ptr<EnemyBase> pEnemy,float searchRadius) :
	CharacterStateBase(pEnemy),
	searchRadius_(searchRadius),
	pEnemy_(pEnemy)
{
}

bool EnemyStateBase::PlayerSearchDistance(float radius) const
{
	//敵の参照
	auto enemy = pEnemy_.lock();
	//敵が存在しない場合はreturn
	if (!enemy) return false;

	//敵・プレイヤーそれぞれの位置を取得
	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//敵からプレイヤーへのベクトルを計算
	Vector3 toPlayer = playerPos - enemyPos;
	//距離を計算
	float distSq = (toPlayer.x_ * toPlayer.x_) + (toPlayer.y_ * toPlayer.y_) + (toPlayer.z_ * toPlayer.z_);

	float radiusSq = radius * radius;
	//E->Pの距離がE+Pより小さかったら検知する
	return distSq <= radiusSq;
}
