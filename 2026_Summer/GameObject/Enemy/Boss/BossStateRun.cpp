#include "BossStateRun.h"
#include "BossStateAttack.h"
#include "Boss.h"
#include <algorithm>

namespace
{
	//旋回している時の移動速度
	constexpr float kOrbitSpeed = 150.0f;

	//距離のズレを補正する速度(放射方向)
	constexpr float kRadialCorrectSpeed = 80.0f;

	//線形補間の割合
	constexpr float kRotateLerpRate = 0.3f;

	//維持したいプレイヤーとの距離(様子をうかがう距離)
	constexpr float kOrbitDistance = 400.0f;

	//このズレ幅で補正速度が最大になる(比例配分の基準)
	constexpr float kMaxCorrectRange = 150.0f;
}

BossStateRun::BossStateRun(std::weak_ptr<Boss> pBoss, float searchRadius) :
	BossStateBase(pBoss, searchRadius)
{
}

void BossStateRun::Enter()
{
	auto boss = pBoss_.lock();
	if (!boss)return;

	//走りアニメーションへ遷移
	boss->ChangeAnimation(AnimationState::Run);

	isClockwise_ = (std::rand() % 2) == 0;
}

void BossStateRun::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	auto boss = pBoss_.lock();
	if (!boss)return;

	//攻撃のクールタイムが来たら
	if (boss->IsAttackReady())
	{
		//攻撃状態に遷移
		auto nextState = std::make_shared<BossStateAttack>(pBoss_, searchRadius_);
		boss->ChangeState(nextState);
		return;
	}

	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//プレイヤーからボスへの差分ベクトル
	Vector3 fromPlayer = enemyPos - playerPos;
	fromPlayer.y_ = 0.0f;

	float distance = fromPlayer.Length();
	//ほぼ重なっている場合は移動処理をしない
	if (distance < 0.0001f) return;

	//プレイヤーから見た放射方向(外向き)
	Vector3 radialDir = fromPlayer.Normalize();

	//接線方向(旋回方向)
	Vector3 tangentDir = { -radialDir.z_, 0.0f, radialDir.x_ };
	if (!isClockwise_)
	{
		tangentDir = tangentDir * -1.0f;
	}

	//適正距離からのズレ
	float diff = distance - kOrbitDistance;

	//ズレ量に応じて補正速度を比例配分
	float correctRate = std::clamp(diff / kMaxCorrectRange, -1.0f, 1.0f);

	//放射方向の補正速度
	Vector3 radialVelocity = radialDir * (-correctRate * kRadialCorrectSpeed);

	//接線方向の旋回速度
	Vector3 tangentVelocity = tangentDir * kOrbitSpeed;

	//二つを合成
	Vector3 moveVec = radialVelocity + tangentVelocity;
	float moveSpeed = moveVec.Length();

	if (moveSpeed > 0.0001f)
	{
		Vector3 moveDir = moveVec.Normalize();
		ApplyMove(enemy, enemyPos, moveDir, moveSpeed);
	}

	//向きは移動方向ではなく常にプレイヤーの方を向かせる
	Vector3 toPlayerDir = playerPos - enemyPos;
	toPlayerDir.y_ = 0.0f;
	toPlayerDir.Normalize();
	enemy->SetMoveAngle(RotateAngle(enemy->GetMoveAngle(), toPlayerDir, kRotateLerpRate));
}

void BossStateRun::Exit()
{
}
