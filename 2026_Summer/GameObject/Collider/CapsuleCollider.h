#pragma once
#include "Collider.h"
#include "GameObject.h"
class CapsuleCollider :public Collider
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="radius">カプセルの半径</param>
	/// <param name="height">カプセルのheight</param>
	/// <param name="centerPos">中心座標</param>
	CapsuleCollider(float radius, float height, Vector3 centerPos);
	virtual~CapsuleCollider()override;

	void Update()override;

	Vector3 GetWorldA()const { return worldA_; }
	Vector3 GetWorldB()const { return worldB_; }
	float GetRadius()const { return radius_; }

	/// <summary>
	/// ワールド座標のセット
	/// </summary>
	/// <param name="worldA">ワールド座標A</param>
	/// <param name="worldb">ワールド座標B</param>
	void SetWorldPos(const Vector3& worldA, const Vector3& worldB);

	/// <summary>
	/// ワールド座標の計算を使用の有無
	/// </summary>
	/// <param name="useWPos">ワールド座標を使用するかどうか</param>
	bool SetUseWorldPos(bool useWPos);

private:
	float radius_;           //半径
	float height_;           //筒の高さ
	bool useWorldPos_;
	Vector3 centerOffset_;   //中心からのオフセット

	Vector3 worldA_;         //下側の球の中心点
	Vector3 worldB_;         //上側の球の中心点
};