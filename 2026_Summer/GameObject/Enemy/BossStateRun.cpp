#include "BossStateRun.h"
#include "BossStateAttack.h"
#include "Boss.h"

namespace
{
	//プレイヤーへ近づく時の移動速度
	constexpr float kApproachSpeed = 300.0f;

	//旋回している時の移動速度
	constexpr float kOrbitSpeed = 150.0f;
	//線形補間の割合
	constexpr float kRotateLerpRate = 0.3f;

	//維持したいプレイヤーとの距離
	constexpr float kOrbitDistance = 400.0f;

	//距離判定にヒステリシスを持たせるための余裕
	constexpr float kOrbitMargin = 50.0f;
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

	//攻撃クールタイムが経過していたら攻撃状態に遷移する
	if (boss->IsAttackReady())
	{
		auto nextState = std::make_shared<BossStateAttack>(pBoss_, searchRadius_);
		boss->ChangeState(nextState);
		return;
	}

	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//プレイヤー→ボスへのベクトル(距離維持の基準)
	Vector3 fromPlayer = enemyPos - playerPos;
	fromPlayer.y_ = 0.0f;

	float distance = fromPlayer.Length();

	//ほぼ重なっている場合は移動処理をしない
	if (distance < 0.0001f) return;

	//プレイヤーから見た放射方向(外向き)
	Vector3 radialDir = fromPlayer.Normalize();

	Vector3 moveDir;
	float moveSpeed;

	if (distance > kOrbitDistance + kOrbitMargin)
	{
		//離れすぎているので近づく
		moveDir = radialDir * -1.0f;
		moveSpeed = kApproachSpeed;
	}
	else if (distance < kOrbitDistance - kOrbitMargin)
	{
		//近すぎるので離れる
		moveDir = radialDir;
		moveSpeed = kApproachSpeed;
	}
	else
	{
		//適正距離内なのでプレイヤーの周りを旋回する(放射方向と垂直なベクトル)
		Vector3 tangentDir = { -radialDir.z_, 0.0f, radialDir.x_ };
		if (!isClockwise_)
		{
			tangentDir = tangentDir * -1.0f;
		}
		moveDir = tangentDir;
		moveSpeed = kOrbitSpeed;
	}

	moveDir.Normalize();

	//速度・位置の適用
	ApplyMove(enemy, enemyPos, moveDir, moveSpeed);

	//向きは移動方向ではなく常にプレイヤーの方を向かせる
	Vector3 toPlayerDir = playerPos - enemyPos;
	toPlayerDir.y_ = 0.0f;
	toPlayerDir.Normalize();
	enemy->SetMoveAngle(RotateAngle(enemy->GetMoveAngle(), toPlayerDir, kRotateLerpRate));
}

void BossStateRun::Exit()
{
}
