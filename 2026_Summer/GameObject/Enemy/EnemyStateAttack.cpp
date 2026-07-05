#include "EnemyStateAttack.h"
#include "EnemyStateIdle.h"
#include "EnemyBase.h"

EnemyStateAttack::EnemyStateAttack(std::weak_ptr<EnemyBase> pEnemy,float searchRadius):
	EnemyStateBase(pEnemy,searchRadius)
{
}

void EnemyStateAttack::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//速度をゼロにする
	enemy->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
}

void EnemyStateAttack::Update()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//攻撃のサブステートの更新
	//ここで攻撃の予備動作、攻撃、隙状態の更新を実装していく感じにする
	if (pCurrentAttackState_)
	{
		pCurrentAttackState_->Update();
	}

	//攻撃アニメーションが終了したら
	if (enemy->IsAnimationEnd()&&!pCurrentAttackState_)
	{
		//Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
	}
}

void EnemyStateAttack::Exit()
{
	if (pCurrentAttackState_)
	{
		pCurrentAttackState_->Exit();
		pCurrentAttackState_ = nullptr;
	}
}

void EnemyStateAttack::ChangeAttackState(std::shared_ptr<EnemyAttackSubStateBase> nextAttackState)
{
	//残っているステートがあるならそのステートの終了処理を呼ぶ
	if (pCurrentAttackState_)
	{
		pCurrentAttackState_->Exit();
	}

	//次の状態に遷移
	pCurrentAttackState_ = nextAttackState;

	//ステートがあるなら開始処理を呼ぶ
	if (pCurrentAttackState_)
	{
		pCurrentAttackState_->Enter();
	}
}
