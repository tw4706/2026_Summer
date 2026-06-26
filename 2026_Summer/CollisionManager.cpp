#include "CollisionManager.h"
#include "Collider/Collider.h"
#include "Collider/Collidable.h"
#include "Collider/PolygonCollider.h"
#include "Collider/CapsuleCollider.h"
#include"Player/Player.h"
#include"Vector3.h"
#include "Katana.h"
#include "Stage.h"
#include <cmath>
#include <Dxlib.h>

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
	//要素のクリア
	pAllColliders_.clear();
}

void CollisionManager::RegisterCollider(Collider* pCollider)
{
	if (!pCollider) return;

	//登録だけなので追加するだけ
	pAllColliders_.push_back(pCollider);
}

void CollisionManager::UnRegisterCollider(Collider* pCollider)
{
	//配列から削除する
	for (auto it = pAllColliders_.begin(); it != pAllColliders_.end(); ++it)
	{
		if (*it == pCollider)
		{
			pAllColliders_.erase(it);
			break;
		}
	}
}

void CollisionManager::UpdateCheckCollision()
{
	//コライダーの更新を行う
	for (auto pCollider : pAllColliders_)
	{
		if (pCollider)
		{
			Collidable* pOwner = pCollider->GetOwner();
			if (pOwner) pOwner->SetIsGround(false);
			pCollider->Update();
		}
	}

	for (size_t i = 0; i < pAllColliders_.size(); ++i)
	{
		for (size_t j = i + 1; j < pAllColliders_.size(); ++j)
		{
			Collider* pColA = pAllColliders_[i];
			Collider* pColB = pAllColliders_[j];

			if (!pColA || !pColB) continue;

			Collidable* pObjA = pColA->GetOwner();
			Collidable* pObjB = pColB->GetOwner();

			//同じオブジェクトのコライダー同士ならスキップ
			if (pObjA == pObjB) continue;

			//刀と、その刀の持ち主との衝突はスキップ
			if (Katana* pKatanaA = dynamic_cast<Katana*>(pObjA))
			{
				if (pKatanaA->GetOwnerCharacter() == pObjB) continue;
			}
			if (Katana* pKatanaB = dynamic_cast<Katana*>(pObjB))
			{
				if (pKatanaB->GetOwnerCharacter() == pObjA) continue;
			}

			//球と球
			if (pColA->GetType() == ColliderType::Sphere && pColB->GetType() == ColliderType::Sphere)
			{
				CheckSphereVsSphere(pObjA, pObjB);
			}
			//球とカプセル
			else if (pColA->GetType() == ColliderType::Sphere && pColB->GetType() == ColliderType::Capsule)
			{
				CheckSphereVsCapsule(pObjA, pObjB);
			}
			//カプセルと球
			else if (pColA->GetType() == ColliderType::Capsule && pColB->GetType() == ColliderType::Sphere)
			{
				CheckSphereVsCapsule(pObjB, pObjA);
			}
			//カプセルとカプセル
			else if (pColA->GetType() == ColliderType::Capsule && pColB->GetType() == ColliderType::Capsule)
			{
				CheckCapsuleVsCapsule(pObjA, pObjB);
			}
			//カプセルとポリゴン
			else if (pColA->GetType() == ColliderType::Capsule && pColB->GetType() == ColliderType::Polygon)
			{
				CheckCapsuleVsPolygon(pObjA, pObjB);
			}
			//ポリゴンとカプセル
			else if (pColA->GetType() == ColliderType::Polygon && pColB->GetType() == ColliderType::Capsule)
			{
				CheckCapsuleVsPolygon(pObjB, pObjA);
			}
		}
	}
}

bool CollisionManager::CheckSphereVsCapsule(Collidable* pSphereObj, Collidable* pCapsuleObj)
{
	if (!pSphereObj || !pCapsuleObj) return false;

	// 各オブジェクトから必要なコライダーデータを取得
	Vector3 sphereCenter = pSphereObj->GetPos();
	float sphereRadius = 0.5f;

	const auto& colliders = pCapsuleObj->GetColliders();
	if (colliders.empty()) return false;
	CapsuleCollider* pCap = static_cast<CapsuleCollider*>(colliders[0].get());

	Vector3 capA = pCap->GetWorldA();
	Vector3 capB = pCap->GetWorldB();
	float capRadius = pCap->GetRadius();

	// ベクトルを計算
	Vector3 ab = { capB.x_ - capA.x_, capB.y_ - capA.y_, capB.z_ - capA.z_ };
	Vector3 ap = { sphereCenter.x_ - capA.x_, sphereCenter.y_ - capA.y_, sphereCenter.z_ - capA.z_ };

	float dot = ap.x_ * ab.x_ + ap.y_ * ab.y_ + ap.z_ * ab.z_;
	float abLenSq = ab.x_ * ab.x_ + ab.y_ * ab.y_ + ab.z_ * ab.z_;

	float t = (abLenSq > 0.0f) ? (dot / abLenSq) : 0.0f;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	Vector3 closestPointC;
	closestPointC.x_ = capA.x_ + t * ab.x_;
	closestPointC.y_ = capA.y_ + t * ab.y_;
	closestPointC.z_ = capA.z_ + t * ab.z_;

	float dx = sphereCenter.x_ - closestPointC.x_;
	float dy = sphereCenter.y_ - closestPointC.y_;
	float dz = sphereCenter.z_ - closestPointC.z_;
	float distSq = dx * dx + dy * dy + dz * dz;

	float radSum = sphereRadius + capRadius;

	// 当たっていなければ終了
	if (distSq > (radSum * radSum)) return false;

	// 衝突時の通知
	pSphereObj->OnCollision(pCapsuleObj);
	pCapsuleObj->OnCollision(pSphereObj);

	return true;
}

bool CollisionManager::CheckSphereVsSphere(Collidable* pSphereObjA, Collidable* pSphereObjB)
{
	if (!pSphereObjA || !pSphereObjB) return false;

	Vector3 posA = pSphereObjA->GetPos();
	Vector3 posB = pSphereObjB->GetPos();
	float radA = 0.5f;
	float radB = 0.5f;

	float dx = posA.x_ - posB.x_;
	float dy = posA.y_ - posB.y_;
	float dz = posA.z_ - posB.z_;
	float distSq = dx * dx + dy * dy + dz * dz;

	float radSum = radA + radB;

	// 当たっていなければ終了
	if (distSq > (radSum * radSum)) return false;

	pSphereObjA->OnCollision(pSphereObjB);
	pSphereObjB->OnCollision(pSphereObjA);

	return true;
}

bool CollisionManager::CheckCapsuleVsCapsule(Collidable* pCapsuleObjA, Collidable* pCapsuleObjB)
{
	if (!pCapsuleObjA || !pCapsuleObjB) return false;

	const auto& colsA = pCapsuleObjA->GetColliders();
	const auto& colsB = pCapsuleObjB->GetColliders();
	if (colsA.empty() || colsB.empty()) return false;

	CapsuleCollider* pCapA = static_cast<CapsuleCollider*>(colsA[0].get());
	CapsuleCollider* pCapB = static_cast<CapsuleCollider*>(colsB[0].get());

	Vector3 a1 = pCapA->GetWorldA();
	Vector3 a2 = pCapA->GetWorldB();
	Vector3 b1 = pCapB->GetWorldA();
	Vector3 b2 = pCapB->GetWorldB();

	Vector3 d1 = { a2.x_ - a1.x_, a2.y_ - a1.y_, a2.z_ - a1.z_ };
	Vector3 d2 = { b2.x_ - b1.x_, b2.y_ - b1.y_, b2.z_ - b1.z_ };
	Vector3 r = { a1.x_ - b1.x_, a1.y_ - b1.y_, a1.z_ - b1.z_ };

	float f11 = d1.x_ * d1.x_ + d1.y_ * d1.y_ + d1.z_ * d1.z_;
	float f22 = d2.x_ * d2.x_ + d2.y_ * d2.y_ + d2.z_ * d2.z_;
	float f12 = d1.x_ * d2.x_ + d1.y_ * d2.y_ + d1.z_ * d2.z_;

	float g1 = d1.x_ * r.x_ + d1.y_ * r.y_ + d1.z_ * r.z_;
	float g2 = d2.x_ * r.x_ + d2.y_ * r.y_ + d2.z_ * r.z_;

	float s = 0.0f;
	float t = 0.0f;
	float denom = f11 * f22 - f12 * f12;

	if (denom != 0.0f)
	{
		s = (f12 * g2 - f22 * g1) / denom;
		if (s < 0.0f) s = 0.0f;
		if (s > 1.0f) s = 1.0f;
		t = (f12 * s + g2) / f22;
	}
	else
	{
		s = 0.0f;
		t = g2 / f22;
	}

	if (t < 0.0f) { t = 0.0f; s = -g1 / f11; }
	else if (t > 1.0f) { t = 1.0f; s = (f12 - g1) / f11; }

	if (s < 0.0f) s = 0.0f;
	if (s > 1.0f) s = 1.0f;

	Vector3 pointP = { a1.x_ + s * d1.x_, a1.y_ + s * d1.y_, a1.z_ + s * d1.z_ };
	Vector3 pointQ = { b1.x_ + t * d2.x_, b1.y_ + t * d2.y_, b1.z_ + t * d2.z_ };

	float dx = pointP.x_ - pointQ.x_;
	float dy = pointP.y_ - pointQ.y_;
	float dz = pointP.z_ - pointQ.z_;
	float distSq = dx * dx + dy * dy + dz * dz;

	float radSum = pCapA->GetRadius() + pCapB->GetRadius();

	// 当たっていなければ即 false を返す
	if (distSq > (radSum * radSum)) return false;

	// 衝突イベントを通知
	pCapsuleObjA->OnCollision(pCapsuleObjB);
	pCapsuleObjB->OnCollision(pCapsuleObjA);

	// 刀が絡んでいる場合は押し戻しをスキップして終了
	bool isKatanaInvolved = (dynamic_cast<Katana*>(pCapsuleObjA) != nullptr || dynamic_cast<Katana*>(pCapsuleObjB) != nullptr);
	if (isKatanaInvolved) return true;

	float dist = std::sqrt(distSq);
	if (dist > 0.0f)
	{
		float overlap = radSum - dist;
		Vector3 dirBtoA = { (pointP.x_ - pointQ.x_) / dist, (pointP.y_ - pointQ.y_) / dist, (pointP.z_ - pointQ.z_) / dist };
		Vector3 dirAtoB = { -dirBtoA.x_, -dirBtoA.y_, -dirBtoA.z_ };

		float weightA = 0.5f;
		float weightB = 0.5f;

		// GetOwner()を経由せず、直接キャストを判定できる
		if (dynamic_cast<Player*>(pCapsuleObjA)) { weightA = 1.0f; weightB = 0.0f; }
		else if (dynamic_cast<Player*>(pCapsuleObjB)) { weightA = 0.0f; weightB = 1.0f; }

		Vector3 posA = pCapsuleObjA->GetPos();
		posA.x_ += dirBtoA.x_ * overlap * weightA;
		posA.z_ += dirBtoA.z_ * overlap * weightA;
		pCapsuleObjA->SetPos(posA);

		Vector3 posB = pCapsuleObjB->GetPos();
		posB.x_ += dirAtoB.x_ * overlap * weightB;
		posB.z_ += dirAtoB.z_ * overlap * weightB;
		pCapsuleObjB->SetPos(posB);
	}

	return true;
}

bool CollisionManager::CheckCapsuleVsPolygon(Collidable* pCapsuleObj, Collidable* pPolygonObj)
{
	//コライダーが存在しない場合はfalseを返して何もしない
	if (!pCapsuleObj || !pPolygonObj) return false;

	if (dynamic_cast<Stage*>(pCapsuleObj)) return false;

	//コライダーの配列を取得
	const auto& capCols = pCapsuleObj->GetColliders();
	const auto& polyCols = pPolygonObj->GetColliders();
	if (capCols.empty() || polyCols.empty()) return false;

	//コライダーのポインタをキャストして取得
	CapsuleCollider* pCap = static_cast<CapsuleCollider*>(capCols[0].get());
	PolygonCollider* pPoly = static_cast<PolygonCollider*>(polyCols[0].get());

	//カプセルの始点・終点・半径の取得
	Vector3 capA = pCap->GetWorldA();
	Vector3 capB = pCap->GetWorldB();
	float capRadius = pCap->GetRadius();

	//モデルハンドルの取得
	int modelHandle = pPoly->GetModelHandle();
	if (modelHandle < 0) return false;

	printfDx(L"capA=(%.2f, %.2f, %.2f) capB=(%.2f, %.2f, %.2f) radius=%.2f\n",
		capA.x_, capA.y_, capA.z_,
		capB.x_, capB.y_, capB.z_,
		capRadius);

	//Dxライブラリの関数でカプセルとポリゴンの当たり判定のチェックを行う
	MV1_COLL_RESULT_POLY_DIM result = MV1CollCheck_Capsule(
		modelHandle, -1,
		VGet(capA.x_, capA.y_, capA.z_),
		VGet(capB.x_, capB.y_, capB.z_),
		capRadius);

	bool isHit = (result.HitNum > 0);

	//もし当たっているポリゴンが1つでもあるなら
	if (isHit)
	{
		//衝突処理
		pCapsuleObj->OnCollision(pPolygonObj);
		pPolygonObj->OnCollision(pCapsuleObj);

		//押し戻し処理
		//各ヒットポリゴンの法線方向に押し戻しを行う
		Vector3 pos = pCapsuleObj->GetPos();

		for (int i = 0; i < result.HitNum; ++i)
		{
			MV1_COLL_RESULT_POLY& poly = result.Dim[i];
			VECTOR normal = poly.Normal;

			//法線のY咆哮の傾きで床か壁かを判定する
			if (normal.y > 0.1f)
			{
				float targetCapAY = poly.HitPosition.y + capRadius;
				float diffY = targetCapAY - capA.y_;
				pos.y_ += diffY;
				capA.y_ += diffY;
				capB.y_ += diffY;
				pCapsuleObj->SetIsGround(true);
			}
			else
			{
				//カプセル軸上で、このヒット位置に最も近い点を求める
				Vector3 ab = { capB.x_ - capA.x_, capB.y_ - capA.y_, capB.z_ - capA.z_ };
				Vector3 hitPos = { poly.HitPosition.x, poly.HitPosition.y, poly.HitPosition.z };
				Vector3 ah = { hitPos.x_ - capA.x_, hitPos.y_ - capA.y_, hitPos.z_ - capA.z_ };

				float abLenSq = ab.x_ * ab.x_ + ab.y_ * ab.y_ + ab.z_ * ab.z_;
				float t = (abLenSq > 0.0f) ? ((ah.x_ * ab.x_ + ah.y_ * ab.y_ + ah.z_ * ab.z_) / abLenSq) : 0.0f;
				if (t < 0.0f) t = 0.0f;
				if (t > 1.0f) t = 1.0f;

				Vector3 closest;
				closest.x_ = capA.x_ + t * ab.x_;
				closest.y_ = capA.y_ + t * ab.y_;
				closest.z_ = capA.z_ + t * ab.z_;

				float dx = closest.x_ - hitPos.x_;
				float dy = closest.y_ - hitPos.y_;
				float dz = closest.z_ - hitPos.z_;
				float distAlongNormal = dx * normal.x + dy * normal.y + dz * normal.z;
				float penetration = capRadius - distAlongNormal;

				if (penetration > 0.0f)
				{
					pos.x_ += normal.x * penetration;
					pos.z_ += normal.z * penetration;
					capA.x_ += normal.x * penetration;
					capA.z_ += normal.z * penetration;
					capB.x_ += normal.x * penetration;
					capB.z_ += normal.z * penetration;
				}
			}
		}

		//位置の適用
		pCapsuleObj->SetPos(pos);
	}
	//メモリの解放を行う
	MV1CollResultPolyDimTerminate(result);

	//当たっているかどうかを返す
	return isHit;
}
