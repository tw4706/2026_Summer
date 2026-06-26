#include "PolygonCollider.h"

PolygonCollider::PolygonCollider(int modelHandle):
	Collider(ColliderType::Polygon),
	modelHandle_(modelHandle)
{
}

PolygonCollider::~PolygonCollider()
{
}

void PolygonCollider::Update()
{
}
