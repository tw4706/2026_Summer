#include "Character.h"
#include "CharacterStateBase.h"

Character::Character(Vector3 pos, Vector3 vel, float dir):
	GameObject(pos,vel,dir),
	hp_(0)
{
}

Character::~Character()
{
}

void Character::ChangeState(std::shared_ptr<CharacterStateBase> pNextState)
{
	if (pState_) pState_->Exit();//現在の状態の終了処理を行う
	pState_ = pNextState;
	if (pState_) pState_->Enter();//次の状態の処理を開始する
}
