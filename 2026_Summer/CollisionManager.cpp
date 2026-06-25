#include "CollisionManager.h"
#include "Collider/CapsuleCollider.h"
#include "Collider/Collidable.h"
#include"Player/Player.h"
#include "Character.h"
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

void CollisionManager::UpdateCheckCollision(Stage* pStage)
{
	//コライダーの更新を行う
	for (auto pCollider : pAllColliders_)
	{
		if (pCollider) pCollider->Update();
	}

	//キャラクターとステージの当たり判定
	for (auto pCollider : pAllColliders_)
	{
		if (pCollider->GetType() == ColliderType::Capsule)
		{
			CheckCapsuleVsPolygon(pCollider, pStage);
		}
	}

	for (size_t i = 0; i < pAllColliders_.size(); ++i)
	{
		for (size_t j = i + 1; j < pAllColliders_.size(); ++j)
		{
			Collider* pColA = pAllColliders_[i];
			Collider* pColB = pAllColliders_[j];

			// 同じオブジェクトのコライダー同士ならスキップ
			if (pColA->GetOwner() == pColB->GetOwner()) continue;

			//刀と、その刀の持ち主との衝突はスキップ
			if (Katana* pKatanaA = dynamic_cast<Katana*>(pColA->GetOwner()))
			{
				if (pKatanaA->GetOwnerCharacter() == pColB->GetOwner()) continue;
			}
			if (Katana* pKatanaB = dynamic_cast<Katana*>(pColB->GetOwner()))
			{
				if (pKatanaB->GetOwnerCharacter() == pColA->GetOwner()) continue;
			}

			//球と球
			if (pColA->GetType() == ColliderType::Sphere && pColB->GetType() == ColliderType::Sphere)
			{
				CheckSphereVsSphere(pColA, pColB);
			}
			//球とカプセル
			else if (pColA->GetType() == ColliderType::Sphere && pColB->GetType() == ColliderType::Capsule)
			{
				CheckSphereVsCapsule(pColA, pColB);
			}
			//カプセルと球
			else if (pColA->GetType() == ColliderType::Capsule && pColB->GetType() == ColliderType::Sphere)
			{
				CheckSphereVsCapsule(pColB, pColA);
			}
			//カプセルとカプセル
			else if (pColA->GetType() == ColliderType::Capsule && pColB->GetType() == ColliderType::Capsule)
			{
				CheckCapsuleVsCapsule(pColA, pColB);
			}
		}
	}
}

void CollisionManager::CheckSphereVsCapsule(Collider* pSphere, Collider* pCapsule)
{
	//扱いやすいようにそれぞれの型にキャスト
	Vector3 sphereCenter = pSphere->GetOwner()->GetPos();
	float sphereRadius = 0.5f;

	CapsuleCollider* pCap = static_cast<CapsuleCollider*>(pCapsule);
	Vector3 capA = pCap->GetWorldA();
	Vector3 capB = pCap->GetWorldB();
	float capRadius = pCap->GetRadius();

	//ベクトルを計算
	Vector3 ab = { capB.x_ - capA.x_, capB.y_ - capA.y_, capB.z_ - capA.z_ }; //カプセルの軸
	Vector3 ap = { sphereCenter.x_ - capA.x_, sphereCenter.y_ - capA.y_, sphereCenter.z_ - capA.z_ };

	//線分abに対するベクトルapの射影比率を求める
	// t = (ap ・ ab) / |ab|^2
	float dot = ap.x_ * ab.x_ + ap.y_ * ab.y_ + ap.z_ * ab.z_; //内積 (ap ・ ab)
	float abLenSq = ab.x_ * ab.x_ + ab.y_ * ab.y_ + ab.z_ * ab.z_; //カプセル軸の長さの2乗 |ab|^2

	//カプセルの高さが0ならt=0
	float t = (abLenSq > 0.0f) ? (dot / abLenSq) : 0.0f;

	//クランプ処理
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	//線分ab上で、球の中心に最も近い「最短点C」を割り出す
	Vector3 closestPointC;
	closestPointC.x_ = capA.x_ + t * ab.x_;
	closestPointC.y_ = capA.y_ + t * ab.y_;
	closestPointC.z_ = capA.z_ + t * ab.z_;

	//最短点C」と「球の中心P」の距離の2乗を計算
	float dx = sphereCenter.x_ - closestPointC.x_;
	float dy = sphereCenter.y_ - closestPointC.y_;
	float dz = sphereCenter.z_ - closestPointC.z_;
	float distSq = dx * dx + dy * dy + dz * dz;

	//衝突判定
	float radSum = sphereRadius + capRadius;
	if (distSq <= (radSum * radSum))
	{
		//衝突イベントを通知
		Collidable* pObjA = pSphere->GetOwner();
		Collidable* pObjB = pCapsule->GetOwner();

		if (pObjA) pObjA->OnCollision(pObjB);
		if (pObjB) pObjB->OnCollision(pObjA);
	}
}

void CollisionManager::CheckSphereVsSphere(Collider* pSphereA, Collider* pSphereB)
{
	Vector3 posA = pSphereA->GetOwner()->GetPos();
	Vector3 posB = pSphereB->GetOwner()->GetPos();
	float radA = 0.5f;
	float radB = 0.5f;

	//各成分の距離(x,y,z)
	float dx = posA.x_ - posB.x_;
	float dy = posA.y_ - posB.y_;
	float dz = posA.z_ - posB.z_;
	float distSq = dx * dx + dy * dy + dz * dz;

	float radSum = radA + radB;
	if (distSq <= (radSum * radSum))
	{
		Collidable* pObjA = pSphereA->GetOwner();
		Collidable* pObjB = pSphereB->GetOwner();
		if (pObjA) pObjA->OnCollision(pObjB);
		if (pObjB) pObjB->OnCollision(pObjA);
	}
}

void CollisionManager::CheckCapsuleVsCapsule(Collider* pCapsuleA, Collider* pCapsuleB)
{
	CapsuleCollider* pCapA = static_cast<CapsuleCollider*>(pCapsuleA);
	CapsuleCollider* pCapB = static_cast<CapsuleCollider*>(pCapsuleB);

	//カプセルAの線分
	Vector3 a1 = pCapA->GetWorldA();
	Vector3 a2 = pCapA->GetWorldB();
	//カプセルBの線分
	Vector3 b1 = pCapB->GetWorldA();
	Vector3 b2 = pCapB->GetWorldB();

	//各種方向ベクトル
	Vector3 d1 = { a2.x_ - a1.x_, a2.y_ - a1.y_, a2.z_ - a1.z_ }; //線分Aのベクトル
	Vector3 d2 = { b2.x_ - b1.x_, b2.y_ - b1.y_, b2.z_ - b1.z_ }; //線分Bのベクトル
	Vector3 r = { a1.x_ - b1.x_, a1.y_ - b1.y_, a1.z_ - b1.z_ }; //A1からB1へのベクトル

	//内積を利用した最短詰めの計算用パラメータ
	float f11 = d1.x_ * d1.x_ + d1.y_ * d1.y_ + d1.z_ * d1.z_; // |d1|^2
	float f22 = d2.x_ * d2.x_ + d2.y_ * d2.y_ + d2.z_ * d2.z_; // |d2|^2
	float f12 = d1.x_ * d2.x_ + d1.y_ * d2.y_ + d1.z_ * d2.z_; // d1 ・ d2

	float g1 = d1.x_ * r.x_ + d1.y_ * r.y_ + d1.z_ * r.z_; // d1 ・ r
	float g2 = d2.x_ * r.x_ + d2.y_ * r.y_ + d2.z_ * r.z_; // d2 ・ r

	//線分A上の比率 s, 線分B上の比率 t
	float s = 0.0f;
	float t = 0.0f;

	float denom = f11 * f22 - f12 * f12;

	//2つの線分が平行でない場合
	if (denom != 0.0f)
	{
		s = (f12 * g2 - f22 * g1) / denom;
		if (s < 0.0f) s = 0.0f;
		if (s > 1.0f) s = 1.0f;

		t = (f12 * s + g2) / f22;
	}
	else
	{
		//平行な場合は端点を基準にする
		s = 0.0f;
		t = g2 / f22;
	}

	//clamp処理
	if (t < 0.0f) { t = 0.0f; s = -g1 / f11; }
	else if (t > 1.0f) { t = 1.0f; s = (f12 - g1) / f11; }

	if (s < 0.0f) s = 0.0f;
	if (s > 1.0f) s = 1.0f;

	//線分A上の最短点P
	Vector3 pointP = { a1.x_ + s * d1.x_, a1.y_ + s * d1.y_, a1.z_ + s * d1.z_ };
	//線分B上の最短点Q
	Vector3 pointQ = { b1.x_ + t * d2.x_, b1.y_ + t * d2.y_, b1.z_ + t * d2.z_ };

	//点Pと点Qの距離の2乗
	float dx = pointP.x_ - pointQ.x_;
	float dy = pointP.y_ - pointQ.y_;
	float dz = pointP.z_ - pointQ.z_;
	float distSq = dx * dx + dy * dy + dz * dz;

	//半径の合計の2乗と比較
	float radSum = pCapA->GetRadius() + pCapB->GetRadius();
	if (distSq <= (radSum * radSum))
	{
		Collidable* pObjA = pCapsuleA->GetOwner();
		Collidable* pObjB = pCapsuleB->GetOwner();

		if (pObjA) pObjA->OnCollision(pObjB);
		if (pObjB) pObjB->OnCollision(pObjA);

		//当たり判定を行うオブジェクトどちらか一方が刀型の場合スキップ
		bool isKatanaInvolved = (dynamic_cast<Katana*>(pObjA) != nullptr ||
			dynamic_cast<Katana*>(pObjB) != nullptr);

		if (isKatanaInvolved)
		{
			return;
		}

		//平方根を求める
		float dist = std::sqrt(distSq);

		//完全に中心が一致してしまっている場合
		if (dist > 0.0f)
		{
			//めり込みを計算
			float overlap = radSum - dist;

			//Aから見たBへの方向、Bから見たAへの方向をそれぞれ定義
			Vector3 dirBtoA = {
				(pointP.x_ - pointQ.x_) / dist,
				(pointP.y_ - pointQ.y_) / dist,
				(pointP.z_ - pointQ.z_) / dist
			};
			Vector3 dirAtoB = { -dirBtoA.x_, -dirBtoA.y_, -dirBtoA.z_ };

			//押し戻す比率
			float weightA = 0.5f;
			float weightB = 0.5f;

			//オブジェクトの型をチェックして、プレイヤーなら自分が100%押し戻されるようにする
			if (dynamic_cast<Player*>(pObjA))
			{
				weightA = 1.0f; //プレイヤーAが100%戻る
				weightB = 0.0f; //敵Bは動かない
			}
			else if (dynamic_cast<Player*>(pObjB))
			{
				weightA = 0.0f; //敵Aは動かない
				weightB = 1.0f; //プレイヤーBが100%戻る
			}

			//Aを押し戻す
			if (pObjA)
			{
				Vector3 posA = pObjA->GetPos();
				posA.x_ += dirBtoA.x_ * overlap * weightA;
				posA.z_ += dirBtoA.z_ * overlap * weightA;
				pObjA->SetPos(posA);
			}

			//Bを押し戻す
			if (pObjB)
			{
				Vector3 posB = pObjB->GetPos();
				posB.x_ += dirAtoB.x_ * overlap * weightB;
				posB.z_ += dirAtoB.z_ * overlap * weightB;
				pObjB->SetPos(posB);
			}
		}
	}
}

void CollisionManager::CheckCapsuleVsPolygon(Collider* pCapsule, Stage* pStage)
{
	CapsuleCollider* pCap = static_cast<CapsuleCollider*>(pCapsule);
	Vector3 capA = pCap->GetWorldA();
	Vector3 capB = pCap->GetWorldB();

	//この範囲を地面とみなす
	const float kCheckRange = 10.0f;
	VECTOR start = VGet(capA.x_, capA.y_ + kCheckRange, capA.z_);
	VECTOR end = VGet(capB.x_, capB.y_ - kCheckRange, capB.z_);

	//DXライブラリの関数を用いて線分とポリゴンの当たり判定をチェックする
	MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(pStage->GetHandle(), -1, start, end);

	Collidable* pOwner = pCapsule->GetOwner();
	if (!pOwner)return;

	if (result.HitFlag)
	{
		float groundY = result.HitPosition.y;

		//現在位置を取得
		Vector3 currentPos = pOwner->GetPos();

		//位置の補正
		float offsetY = currentPos.y_ - capA.y_;
		float correctedPosY = groundY + offsetY + pCap->GetRadius();

		//位置の適用
		if (currentPos.y_ <= correctedPosY)
		{
			currentPos.y_ = correctedPosY;
			pOwner->SetPos(currentPos);

			//接地判定をtrueにする
			pOwner->SetIsGround(true);

			//Yの速度を0にする
			Vector3 vel = pOwner->GetVelocity();
			vel.y_ = 0.0f;
			pOwner->SetVelocity(vel);
		}
		else
		{
			pOwner->SetIsGround(false);
		}
	}
}