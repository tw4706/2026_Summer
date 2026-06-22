#include "Collider.h"

Collider::Collider(ColliderType type):
	type_(type),
	pOwner_(nullptr)
{
}

void Collider::SetOwner(Collidable* owner)
{
}
