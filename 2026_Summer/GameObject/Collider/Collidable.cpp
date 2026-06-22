#include "Collidable.h"
#include "Collider.h"

Collidable::Collidable(Vector3 pos, Vector3 vel, float dir):
	GameObject(pos,vel,dir)
{
}

Collidable::~Collidable()
{
}

void Collidable::Update()
{
	//各コライダーの更新
	for (auto& pCollider : colliders_)
	{
		if (pCollider)
		{
			pCollider->Update();
		}
	}
}

void Collidable::AddCollider(std::unique_ptr<Collider> pCollider)
{
	if (!pCollider)return;

	pCollider->SetOwner(this);
	colliders_.push_back(std::move(pCollider));
}

const std::vector<std::unique_ptr<Collider>>& Collidable::GetColliders() const
{
	return colliders_;
}
