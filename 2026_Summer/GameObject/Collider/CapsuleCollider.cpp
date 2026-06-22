#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider(float radius, float height, Vector3 centerPos):
	Collider(ColliderType::Capsule),
	radius_(radius),
	height_(height),
	centerOffset_(centerPos),
	worldA_({ 0.0f, 0.0f, 0.0f }),
	worldB_({ 0.0f, 0.0f, 0.0f })
{
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::Update()
{
}
