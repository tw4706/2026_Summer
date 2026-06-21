#include "EnemyStateIdle.h"
#include "EnemyStateRun.h"
#include "EnemyBase.h"
namespace
{
	//敵アニメーション
	//Idle
	const wchar_t* kEnemyIdle = L"Oni|Idle";
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

#ifdef _DEBUG
	DrawFormatString(0, 220, GetColor(255, 255, 255), L"EnemyState: Idle");
	Vector3 ePos = enemy->GetPos();
	Vector3 pPos = enemy->GetPlayerPos();
	float dist = sqrtf((pPos.x_ - ePos.x_) * (pPos.x_ - ePos.x_) +
		(pPos.y_ - ePos.y_) * (pPos.y_ - ePos.y_) +
		(pPos.z_ - ePos.z_) * (pPos.z_ - ePos.z_));
	DrawFormatString(0, 240, GetColor(255, 255, 255), L"EnemyPos: %.2f,%.2f,%.2f", ePos.x_, ePos.y_, ePos.z_);
	DrawFormatString(0, 260, GetColor(255, 255, 255), L"PlayerPos: %.2f,%.2f,%.2f", pPos.x_, pPos.y_, pPos.z_);
	DrawFormatString(0, 280, GetColor(255, 255, 255), L"Distance: %.2f", dist);
#endif

	if (PlayerSearchDistance(200.0f))
	{
		//検知範囲に入ったらRun状態へ遷移
		auto nextState = std::make_shared<EnemyStateRun>(pEnemy_);
		enemy->ChangeState(nextState);
		return;
	}
}

void EnemyStateIdle::Exit()
{
}
