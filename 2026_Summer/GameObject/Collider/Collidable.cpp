#include "Collidable.h"
#include "Collider.h"
#include "CollisionManager.h"

Collidable::Collidable(Vector3 pos, Vector3 vel, float dir):
	GameObject(pos,vel,dir)
{
}

Collidable::~Collidable()
{
	for (const auto& pCollider : colliders_)
	{
		CollisionManager::GetInstance().UnRegisterCollider(pCollider.get());
	}
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

	pCollider->SetOwner(*this);
	Collider* pRawPtr = pCollider.get();
	colliders_.push_back(std::move(pCollider));

	CollisionManager::GetInstance().RegisterCollider(pRawPtr);
}

const std::vector<std::unique_ptr<Collider>>& Collidable::GetColliders() const
{
	return colliders_;
}
