#include "SphereCollider.h"

SphereCollider::SphereCollider(float r):
	Collider(ColliderType::Sphere),
	r_(r)
{
}

SphereCollider::~SphereCollider()
{
}
