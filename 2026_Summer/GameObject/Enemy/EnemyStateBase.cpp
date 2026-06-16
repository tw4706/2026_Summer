#include "EnemyStateBase.h"
#include "Enemy/EnemyBase.h"
#include "Vector3.h"
#include "Input.h"
#include "Camera/CameraBase.h"

EnemyStateBase::EnemyStateBase(std::weak_ptr<EnemyBase> pEnemy):
	CharacterStateBase(pEnemy)
{
}
