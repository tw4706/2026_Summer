#pragma once
#include "Collider/Collidable.h"
#include"Model.h"
#include"Animation.h"

class Matrix4x4;
class Character;
class CapsuleCollider;
class Katana :public Collidable
{
public:
	Katana(Vector3 pos, Vector3 vel, float dir);
	virtual~Katana();

	void Init()override;
	void Update(const MATRIX&handMat, AnimationState ownerState);
	void Draw()override;

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="coll"></param>
	void OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)override;

	/// <summary>
	/// 刀の当たり判定の有効/無効を設定する
	/// </summary>
	void SetColliderEnabled(bool isEnabled);

	/// <summary>
	/// 刀を持っているキャラクターを設定する（自身への衝突を除外するため）
	/// </summary>
	void SetOwnerCharacter(Character* pOwner) { pOwnerCharacter_ = pOwner; }
	Character* GetOwnerCharacter() const { return pOwnerCharacter_; }

private:
	bool isAttacked_ = false;//攻撃したかどうか

	Model katanaModel_;//刀のモデル
	MATRIX worldMat_;//刀のワールド行列
	std::shared_ptr<CapsuleCollider> pCollider_;//コライダー
	Character* pOwnerCharacter_ = nullptr;   //刀を持っているキャラクター
};

