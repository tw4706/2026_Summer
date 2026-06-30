#include "EnemyStateIdle.h"
#include "EnemyStateRun.h"
#include "EnemyBase.h"
namespace
{
	//õ“G”ÍˆÍ‚Ì”¼Œa
	const float kDebugSearchRadius = 500.0f;
}


EnemyStateIdle::EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateIdle::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//IdleƒAƒjƒ[ƒVƒ‡ƒ“‚ÉØ‚è‘Ö‚¦‚é
	enemy->ChangeAnimation(AnimationState::Idle);

	//‘¬“x‚ğƒ[ƒ‚É‚·‚é
	enemy->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//õ“G”ÍˆÍ‚©‚Â‚Ü‚¾“–‚½‚Á‚Ä‚¢‚È‚©‚Á‚½‚ç
	if (PlayerSearchDistance(searchRadius_))
	{
		//Runó‘Ô‚Ö‘JˆÚ
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
		return;
	}
}

void EnemyStateIdle::Exit()
{
}
