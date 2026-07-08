#include "Character.h"
#include "CharacterStateBase.h"

Character::Character(Vector3 pos, Vector3 vel, float dir):
	Collidable(pos,vel,dir),
	hp_(0),
	gravity_(0.7f)
{
}

Character::~Character()
{
	model_.Release();
}

void Character::ChangeState(std::shared_ptr<CharacterStateBase> pNextState)
{
	if (!pNextState) return;

	//ステートが残っている際はそのステートのExitを呼ぶ
	if (pCurrentState_)
	{
		pCurrentState_->Exit();
	}

	//次のステートを更新
	pCurrentState_ = pNextState;
	//初期化処理を行う
	pCurrentState_->Enter();
}

void Character::ChangeAnimation(AnimationState state,const std::wstring&animName)
{
	if (animation_.GetState() != state)
	{
		animation_.ChangeState(state,animName);
	}
	else if(state==AnimationState::Attack)
	{
		animation_.ResetAnimation();
	}
}

void Character::ChangeAnimation(AnimationState state)
{
	animation_.ChangeState(state);
}
