#include "SphereCollider.h"

SphereCollider::SphereCollider(float r):
	Collider(ColliderType::Sphere),
	radius_(r)
{
}

SphereCollider::~SphereCollider()
{
}
