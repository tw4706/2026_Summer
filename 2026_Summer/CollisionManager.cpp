#include "CollisionManager.h"
#include "Collider/Collider.h"
#include "Collider/Collidable.h"
#include "Collider/PolygonCollider.h"
#include "Collider/CapsuleCollider.h"
#include "Collider/SphereCollider.h"
#include"Player/Player.h"
#include"Enemy/EnemyBase.h"
#include"Math/Vector3.h"
#include "Katana.h"
#include "Stage.h"
#include <cmath>
#include <Dxlib.h>

CollisionManager& CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return instance;
}

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
			pCollider->GetOwner().SetIsGround(false);
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

			Collidable& pObjA = pColA->GetOwner();
			Collidable& pObjB = pColB->GetOwner();
			if (&pObjA == &pObjB) continue;

			//敵のHPが0以下の時は当たり判定を行わない
			if (EnemyBase* pEnemyA = dynamic_cast<EnemyBase*>(&pObjA))
			{
				if (pEnemyA->GetHP() <= 0) continue;
				if (pEnemyA->IsReacting()) continue;
			}
			if (EnemyBase* pEnemyB = dynamic_cast<EnemyBase*>(&pObjB))
			{
				if (pEnemyB->GetHP() <= 0) continue;
				if (pEnemyB->IsReacting()) continue;
			}

			//刀と、その刀の持ち主との衝突はスキップ
			if (Katana* pKatanaA = dynamic_cast<Katana*>(&pObjA))
			{
				if (pKatanaA->GetOwnerCharacter() == &pObjB) continue;
			}
			if (Katana* pKatanaB = dynamic_cast<Katana*>(&pObjB))
			{
				if (pKatanaB->GetOwnerCharacter() == &pObjA) continue;
			}


			ColliderType typeA = pColA->GetType();
			ColliderType typeB = pColB->GetType();

			switch (typeA)
			{
			case ColliderType::Sphere:
				switch (typeB)
				{
				case ColliderType::Sphere:
					CheckSphereVsSphere(pObjA, pObjB);
					break;
				case ColliderType::Capsule:
					CheckSphereVsCapsule(pObjA, pObjB);
					break;
				}
				break;

			case ColliderType::Capsule:
				switch (typeB)
				{
				case ColliderType::Sphere:
					CheckSphereVsCapsule(pObjB, pObjA);
					break;
				case ColliderType::Capsule:
					CheckCapsuleVsCapsule(pObjA, pObjB);
					break;
				case ColliderType::Polygon:
					CheckCapsuleVsPolygon(pObjA, pObjB);
					break;
				}
				break;

			case ColliderType::Polygon:
				if (typeB == ColliderType::Capsule)
					CheckCapsuleVsPolygon(pObjB, pObjA);
				break;
			}
		}
	}
}

bool CollisionManager::CheckSphereVsCapsule(Collidable& pSphereObj, Collidable& pCapsuleObj)
{
	if (!&pSphereObj || !&pCapsuleObj) return false;

	SphereCollider* pSphere = nullptr;
	for (const auto& col : pSphereObj.GetColliders())
	{
		if (col->GetType() == ColliderType::Sphere)
		{
			pSphere = static_cast<SphereCollider*>(col.get());
			break;
		}
	}
	if (!pSphere) return false;

	CapsuleCollider* pCap = nullptr;
	for (const auto& col : pCapsuleObj.GetColliders())
	{
		if (col->GetType() == ColliderType::Capsule)
		{
			pCap = static_cast<CapsuleCollider*>(col.get());
			break;
		}
	}
	if (!pCap) return false;

	Vector3 sphereCenter = pSphere->GetPos();
	float sphereRadius = pSphere->GetRadius();

	Vector3 capA = pCap->GetWorldA();
	Vector3 capB = pCap->GetWorldB();
	float capRadius = pCap->GetRadius();

	//ベクトルを計算
	Vector3 ab = capB - capA;
	Vector3 ap = sphereCenter - capA;

	//内積の計算
	float dot = ap.Dot(ab);

	//長さの二乗の計算
	float abLenSq = ab.LengthSq();

	//クランプ処理
	float t = (abLenSq > 0.0f) ? (dot / abLenSq) : 0.0f;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	Vector3 closestPointC = capA + (ab * t);

	float distSq = (sphereCenter - closestPointC).LengthSq();
	float radSum = sphereRadius + capRadius;

	//当たっていなければ終了
	if (distSq > (radSum * radSum)) return false;

	//衝突したときに呼び出される
	pSphereObj.OnCollision(pCapsuleObj,pSphere,pCap);
	pCapsuleObj.OnCollision(pSphereObj,pCap,pSphere);

	return true;
}

bool CollisionManager::CheckSphereVsSphere(Collidable& pSphereObjA, Collidable& pSphereObjB)
{
	if (!&pSphereObjA || !&pSphereObjB) return false;

	SphereCollider* pSphereA = nullptr;
	for (const auto& col : pSphereObjA.GetColliders())
	{
		if (col->GetType() == ColliderType::Sphere)
		{
			pSphereA = static_cast<SphereCollider*>(col.get()); break;
		}
	}
	SphereCollider* pSphereB = nullptr;
	for (const auto& col : pSphereObjB.GetColliders())
	{
		if (col->GetType() == ColliderType::Sphere)
		{
			pSphereB = static_cast<SphereCollider*>(col.get()); break;
		}
	}
	if (!pSphereA || !pSphereB) return false;

	Vector3 posA = pSphereA->GetPos();
	Vector3 posB = pSphereB->GetPos();
	float radA = pSphereA->GetRadius();
	float radB = pSphereB->GetRadius();

	//ABベクトルの距離とその2乗を求める
	float distSq = (posA - posB).LengthSq();
	float radSum = radA + radB;

	//当たっていなければ終了
	if (distSq > (radSum * radSum)) return false;

	pSphereObjA.OnCollision(pSphereObjB,pSphereA, pSphereB);
	pSphereObjB.OnCollision(pSphereObjA, pSphereB, pSphereA);

	return true;
}

bool CollisionManager::CheckCapsuleVsCapsule(Collidable& pCapsuleObjA, Collidable& pCapsuleObjB)
{
	if (!&pCapsuleObjA || !&pCapsuleObjB) return false;

	const auto& colsA = pCapsuleObjA.GetColliders();
	const auto& colsB = pCapsuleObjB.GetColliders();
	if (colsA.empty() || colsB.empty()) return false;

	CapsuleCollider* pCapA = static_cast<CapsuleCollider*>(colsA[0].get());
	CapsuleCollider* pCapB = static_cast<CapsuleCollider*>(colsB[0].get());

	Vector3 a1 = pCapA->GetWorldA();
	Vector3 a2 = pCapA->GetWorldB();
	Vector3 b1 = pCapB->GetWorldA();
	Vector3 b2 = pCapB->GetWorldB();

	//AとBの半径の合計
	float radSum = pCapA->GetRadius() + pCapB->GetRadius();

	//最小距離の二乗を記憶する変数
	float minDistanceSq = 1000000.0f;
	Vector3 pointP, pointQ; //最終的なお互いの最近点

	//垂線チェックループ
	for (int i = 0; i < 2; ++i)
	{
		// i=0 の時は線分B(b1->b2)に対してカプセルAの端点を下ろす
		// i=1 の時は線分A(a1->a2)に対してカプセルBの端点を下ろす
		Vector3 lineStart = (i == 0) ? b1 : a1;
		Vector3 lineEnd = (i == 0) ? b2 : a2;
		Vector3 ab = lineEnd - lineStart;
		float abSqMag = ab.LengthSq();

		for (int j = 0; j < 2; ++j)
		{
			//調べる対象の点（カプセルの端点）
			Vector3 checkPoint;
			if (i == 0) checkPoint = (j == 0) ? a1 : a2;
			else        checkPoint = (j == 0) ? b1 : b2;

			//点から線分の始点へのベクトル
			Vector3 ap = checkPoint - lineStart;

			//内積から垂線の位置を計算してクランプする
			float t = (abSqMag > 0.0f) ? ap.Dot(ab) / abSqMag : 0.0f;
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;

			//線分上の最短座標
			Vector3 minPos = lineStart + (ab * t);
			float distSq = (checkPoint - minPos).LengthSq();

			//記録されている最小距離より短ければ更新
			if (distSq < minDistanceSq)
			{
				minDistanceSq = distSq;
				if (i == 0)
				{
					pointP = checkPoint; //カプセルA上の点
					pointQ = minPos;     //カプセルB上の点
				}
				else
				{
					pointP = minPos;     //カプセルA上の点
					pointQ = checkPoint; //カプセルB上の点
				}
			}
		}
	}

	//当たっていなければ終了
	if (minDistanceSq > (radSum * radSum)) return false;

	//衝突したときに呼び出される
	pCapsuleObjA.OnCollision(pCapsuleObjB, pCapA, pCapB);
	pCapsuleObjB.OnCollision(pCapsuleObjA, pCapB, pCapA);

	//刀が場合は押し戻しをせずに終了する
	bool isKatanaInvolved = (dynamic_cast<Katana*>(&pCapsuleObjA) != nullptr || dynamic_cast<Katana*>(&pCapsuleObjB) != nullptr);
	if (isKatanaInvolved) return true;

	float dist = std::sqrt(minDistanceSq);
	if (dist > 0.0f)
	{
		float overlap = radSum - dist;
		Vector3 dirBtoA = (pointP - pointQ).Normalize();

		float weightA = 0.5f;
		float weightB = 0.5f;

		//直接キャストしたものを判定する
		if (dynamic_cast<Player*>(&pCapsuleObjA)) { weightA = 1.0f; weightB = 0.0f; }
		else if (dynamic_cast<Player*>(&pCapsuleObjB)) { weightA = 0.0f; weightB = 1.0f; }

		//各A,Bの押し戻し位置の適用
		Vector3 posA = pCapsuleObjA.GetPos();
		posA.x_ += dirBtoA.x_ * overlap * weightA;
		posA.z_ += dirBtoA.z_ * overlap * weightA;
		pCapsuleObjA.SetPos(posA);

		Vector3 posB = pCapsuleObjB.GetPos();
		posB.x_ -= dirBtoA.x_ * overlap * weightB;
		posB.z_ -= dirBtoA.z_ * overlap * weightB;
		pCapsuleObjB.SetPos(posB);
	}

	return true;
}

bool CollisionManager::CheckCapsuleVsPolygon(Collidable& pCapsuleObj, Collidable& pPolygonObj)
{
	//コライダーが存在しない場合はfalseを返して何もしない
	if (!&pCapsuleObj || !&pPolygonObj) return false;

	if (dynamic_cast<Stage*>(&pCapsuleObj)) return false;

	//コライダーの配列を取得
	const auto& capCols = pCapsuleObj.GetColliders();
	const auto& polyCols = pPolygonObj.GetColliders();
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

	//Dxライブラリの関数でカプセルとポリゴンの当たり判定のチェックを行う
	MV1_COLL_RESULT_POLY_DIM result = MV1CollCheck_Capsule(modelHandle, -1,
		capA.ToDxlibVector(),capB.ToDxlibVector(),capRadius);

	//当たっているポリゴンが一つ以上あるならtrue,そうでないならfalse
	bool isHit = (result.HitNum > 0);

	//もし当たっているポリゴンが1つでもあるなら
	if (isHit)
	{
		//押し戻し処理
		//各ヒットポリゴンの法線方向に押し戻しを行う
		Vector3 pos = pCapsuleObj.GetPos();

		for (int i = 0; i < result.HitNum; ++i)
		{
			MV1_COLL_RESULT_POLY& poly = result.Dim[i];
			Vector3 normal = poly.Normal;

			//法線のY咆哮の傾きで床か壁かを判定する
			//床の場合
			if (normal.y_ > 0.7f)
			{
				float targetCapAY = poly.HitPosition.y + capRadius;
				float diffY = targetCapAY - capA.y_;
				if (diffY > 0.0f)
				{
					pos.y_ += diffY;
					capA.y_ += diffY;
					capB.y_ += diffY;
				}

				//床方向の速度を0にする
				Vector3 v = pCapsuleObj.GetVelocity();
				float vDotN = v.Dot(normal);
				if (vDotN < 0.0f)
				{
					v -= normal * vDotN;
					pCapsuleObj.SetVelocity(v);
				}

				pCapsuleObj.SetIsGround(true);
			}
			//壁の場合
			else
			{
				//壁の押し戻し
				Vector3 norm = { normal.x_, 0.0f, normal.z_ };
				float normLen = norm.Length();
				if (normLen < 0.0001f) continue;
				norm = norm / normLen;

				// 法線方向への移動量がめり込み量
				Vector3 vel = pCapsuleObj.GetVelocity();
				float moveNormal = -(vel.Dot(norm));
				
				//法線があるなら
				if (moveNormal > 0.0f)
				{
					//法線方向に移動の速度を足す
					Vector3 pushVec = norm * moveNormal;
					pos += pushVec;
					capA += pushVec;
					capB += pushVec;

					//壁方向の速度をゼロにする
					Vector3 v = pCapsuleObj.GetVelocity();
					float vDotN = v.Dot(norm);
					if (vDotN < 0.0f)
					{
						v -= norm * vDotN;
						pCapsuleObj.SetVelocity(v);
					}
				}
			}

			//位置の適用
			pCapsuleObj.SetPos(pos);
		}
		//メモリの解放を行う
		MV1CollResultPolyDimTerminate(result);

		//当たっているかどうかを返す
		return isHit;
	}
}
