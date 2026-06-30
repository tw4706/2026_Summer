#include "Oni.h"
#include "Katana.h"
#include "Matrix4x4.h"
#include"EnemyStateidle.h"
#include"CharacterStateBase.h"
#include "Collider/CapsuleCollider.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	//‰ŠúƒXƒP[ƒ‹
	const Vector3 kFirstScale = { 1.0f, 1.0f, 1.0f };

	//õ“G”ÍˆÍ‚Ì”¼Œa
	const float kDebugSearchRadius = 500.0f;
}

Oni::Oni() :
	EnemyBase()
{
}

Oni::~Oni()
{
}