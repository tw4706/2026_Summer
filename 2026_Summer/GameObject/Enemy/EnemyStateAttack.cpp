#include "EnemyStateAttack.h"
#include "EnemyStateIdle.h"
#include "EnemyAttackSubState/EnemyAttackSubStatePO.h"
#include "EnemyBase.h"
#include "AttackSelect.h"

EnemyStateAttack::EnemyStateAttack(std::weak_ptr<EnemyBase> pEnemy, float searchRadius) :
	EnemyStateBase(pEnemy, searchRadius)
{
}

void EnemyStateAttack::Enter()
{
	auto enemy = pEnemy_.lock();
	if (!enemy)return;

	//速度をゼロにする
	enemy->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });

	//攻撃データを取得
	const auto* pLoader = enemy->GetAttackDataLoader();

	//攻撃データローダーが無ければ何もしない
	if (!pLoader)return;

	//全攻撃候補を取得
	auto attackCandidates = pLoader->GetAllAttackData();

	//重みを持たせた抽選でどの攻撃を出すか決める
	const AttackData* pSelected = AttackSelect::ChooseWeighted(attackCandidates);
	if (!pSelected)return;

	//最初に攻撃の予備動作の状態に遷移してそっから分岐させる
	auto nextAttackState = std::make_shared<EnemyAttackSubStatePO>(pEnemy_, this,*pSelected);
	ChangeAttackState(nextAttackState);
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
	if (enemy->IsAnimationEnd() && !pCurrentAttackState_)
	{
		//Idle状態に遷移
		auto nextState = std::make_shared<EnemyStateIdle>(pEnemy_, searchRadius_);
		enemy->ChangeState(nextState);
	}
}

void EnemyStateAttack::Exit()
{
	//サブステートがここまで存在するなら終了処理を呼び出す	
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
