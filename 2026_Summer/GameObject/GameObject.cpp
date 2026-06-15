#include "GameObject.h"


GameObject::GameObject(Vector3 pos, Vector3 vel,float dir) :
	pos_(pos),
	vel_(vel),
	dir_(dir),
	isDead_(false),
	priority_(0)
{
}

GameObject::~GameObject(){}