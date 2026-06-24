#pragma once
#include "Collider/Collidable.h"
#include"Model.h"
#include"Animation.h"

class Matrix4x4;
class CapsuleCollider;
class Katana :public Collidable
{
public:
	Katana(Vector3 pos, Vector3 vel, float dir);
	virtual~Katana();

	void Init()override;
	void Update()override {};
	void Update(const MATRIX&handMat, AnimationState ownerState);
	void Draw()override;

	void OnCollision(GameObject* obj) override {};

	/// <summary>
	/// 刀の当たり判定の有効/無効を設定する
	/// </summary>
	void SetColliderEnabled(bool isEnabled);

private:
	Model katanaModel_;//刀のモデル
	MATRIX worldMat_;//刀のワールド行列
	std::shared_ptr<CapsuleCollider> pCollider_;//コライダー
};

