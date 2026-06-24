#include "CapsuleCollider.h"
#include "Collidable.h"

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
	if (!pOwner_) return;

	//親オブジェクトのワールド座標を取得
	Vector3 ownerPos = pOwner_->GetPos();

	//カプセルの中心点を計算
	Vector3 baseCenter;
	baseCenter.x_ = ownerPos.x_ + centerOffset_.x_;
	baseCenter.y_ = ownerPos.y_ + centerOffset_.y_;
	baseCenter.z_ = ownerPos.z_ + centerOffset_.z_;

	//カプセルの「高さ（height_）」の半分を上下に振り分ける
	float halfHeight = height_ * 0.5f;

	//下端の球の中心点 (A) ＝ 中心から Y 軸方向に半分下げる
	worldA_.x_ = baseCenter.x_;
	worldA_.y_ = baseCenter.y_ - halfHeight;
	worldA_.z_ = baseCenter.z_;

	//上端の球の中心点 (B) ＝ 中心から Y 軸方向に半分上げる
	worldB_.x_ = baseCenter.x_;
	worldB_.y_ = baseCenter.y_ + halfHeight;
	worldB_.z_ = baseCenter.z_;
}

void CapsuleCollider::SetWorldPos(const Vector3& worldA, const Vector3& worldB)
{
	worldA_ = worldA;
	worldB_ = worldB;
}
