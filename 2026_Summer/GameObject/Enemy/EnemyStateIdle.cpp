#include "EnemyStateIdle.h"
#include "EnemyStateRun.h"
#include "EnemyBase.h"
namespace
{
	//“GƒAƒjƒ[ƒVƒ‡ƒ“
	//Idle
	const wchar_t* kEnemyIdle = L"Oni|Idle";

	//õ“G”ÍˆÍ‚Ì”¼Œa
	const float kDebugSearchRadius = 500.0f;
}


EnemyStateIdle::EnemyStateIdle(std::weak_ptr<EnemyBase> pEnemy) :
	EnemyStateBase(pEnemy)
{
}

void EnemyStateIdle::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	enemy->ChangeAnimation(AnimationState::Idle, kEnemyIdle);

	enemy->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
}

void EnemyStateIdle::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//õ“G”ÍˆÍ‚©‚Â‚Ü‚¾“–‚½‚Á‚Ä‚¢‚È‚©‚Á‚½‚ç
	if (PlayerSearchDistance(kDebugSearchRadius))
	{
		//Runó‘Ô‚Ö‘JˆÚ
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_);
		enemy->ChangeState(nextState);
		return;
	}
}

void EnemyStateIdle::Exit()
{
}
