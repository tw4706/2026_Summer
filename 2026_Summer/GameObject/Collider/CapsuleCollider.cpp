#include "CapsuleCollider.h"
#include "Collidable.h"
#include "Game.h"

CapsuleCollider::CapsuleCollider(float radius, float height, Vector3 centerPos):
	Collider(ColliderType::Capsule),
	radius_(radius),
	height_(height),
	useWorldPos_(false),
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
	if (useWorldPos_) return;
	if (!pOwner_) return;

	//親オブジェクトのワールド座標を取得
	Vector3 ownerPos = pOwner_->GetPos();

	//カプセルの中心点を計算
	Vector3 center;
	center.x_ = ownerPos.x_ + centerOffset_.x_;
	center.y_ = ownerPos.y_ + centerOffset_.y_;
	center.z_ = ownerPos.z_ + centerOffset_.z_;

	//カプセルの高さの半分を上下に振り分ける
	float halfHeight = height_ * Game::kHalf;

	//下端の球の中心点は中心からY軸方向に半分下げる
	worldA_.x_ = center.x_;
	worldA_.y_ = center.y_ - halfHeight;
	worldA_.z_ = center.z_;

	//上端の球の中心点は中心からY軸方向に半分上げる
	worldB_.x_ = center.x_;
	worldB_.y_ = center.y_ + halfHeight;
	worldB_.z_ = center.z_;
}

void CapsuleCollider::SetWorldPos(const Vector3& worldA, const Vector3& worldB)
{
	worldA_ = worldA;
	worldB_ = worldB;
}

bool CapsuleCollider::SetUseWorldPos(bool useWPos)
{
	return useWorldPos_ = useWPos;
}
