#include "EnemyBase.h"
#include "Player/Player.h"


EnemyBase::EnemyBase() :
	Character(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.0f),
	moveAngle_(0.0f)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::OnDamage(int damage)
{
	hp_ -= damage;

	if (hp_ <= 0)
	{
		hp_ = 0;
		Destory();
	}
}

Vector3 EnemyBase::GetPlayerPos() const
{
	auto pPlayer = pPlayer_.lock();

	//もしプレイヤーが存在しない場合は0にする
	if (!pPlayer)
	{
		return Vector3{ 0.0f,0.0f,0.0f };
	}

	//そうでない場合はプレイヤーの位置を返す
	return pPlayer->GetPos();
}

void EnemyBase::SetPlayer(std::weak_ptr<Player> pPlayer)
{
	pPlayer_ = pPlayer;
}
