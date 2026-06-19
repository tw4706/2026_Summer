#include "Character.h"
#include "CharacterStateBase.h"

Character::Character(Vector3 pos, Vector3 vel, float dir):
	GameObject(pos,vel,dir),
	hp_(0),
	isGround_(true),
	gravity_(0.5f)
{
}

Character::~Character()
{
	model_.Release();
}

void Character::ChangeState(std::shared_ptr<CharacterStateBase> pNextState)
{
	if (!pNextState) return;

	if (pCurrentState_)
	{
		pCurrentState_->Exit();
	}

	pCurrentState_ = pNextState;
	pCurrentState_->Enter();
}

void Character::ChangeAnimation(AnimationState state)
{
	if (animation_.GetState() != state)
	{
		animation_.ChangeState(state);
	}
}
