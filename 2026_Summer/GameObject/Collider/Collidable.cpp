#include "Collidable.h"
#include "Collider.h"

Collidable::Collidable(Vector3 pos, Vector3 vel, float dir):
	GameObject(pos,vel,dir)
{
}

Collidable::~Collidable()
{
	//コライダーの削除
	for (auto pCollider : colliders_)
	{
		delete pCollider;
	}

	//コライダーのクリア
	colliders_.clear();
}

void Collidable::Update()
{
	//各コライダーの更新
	for (auto pCollider : colliders_)
	{
		if (pCollider)
		{
			pCollider->Update();
		}
	}
}

void Collidable::AddCollider(Collider* pCollider)
{
	if (!pCollider)return;

	pCollider->SetOwner(this);
	colliders_.push_back(pCollider);
}

const std::vector<Collider*>& Collidable::GetColliders() const
{
	return colliders_;
}
