#pragma once
#include"Vector3.h"

class Collider;
class GameObject
{
public :
	GameObject(Vector3 pos,Vector3 vel,float dir);
	virtual~GameObject();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;

	/// <summary>
	/// 死亡フラグ制御
	/// </summary>
	/// <returns>死んでいるかどうか</returns>
	bool IsDead()const { return isDead_; }

	/// <summary>
	/// 削除処理
	/// </summary>
	void Destory() { isDead_ = true; }

	///ゲッター
	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標の参照</returns>
	const Vector3& GetPos() const { return pos_; }

	/// <summary>
	/// 位置の設定
	/// </summary>
	/// <param name="pos">設定する位置</param>
	void SetPos(const Vector3& pos){ pos_ = pos; }

	/// <summary>
	/// 速度の取得
	/// </summary>
	/// <returns>速度</returns>
	Vector3 GetVelocity() const { return vel_; }

	///セッター
	/// <summary>
	/// 速度のセット
	/// </summary>
	/// <param name="vel">速度</param>
	void SetVelocity(const Vector3& vel) { vel_ = vel; }

	/// <summary>
	/// 優先度の取得
	/// </summary>
	/// <returns>優先度</returns>
	int GetPriority()const { return priority_; }

	/// <summary>
	/// 位置の更新
	/// </summary>
	/// <param name="pos">座標</param>
	void AddPosition() { pos_ += vel_; }

protected:
	Vector3 pos_;	//座標
	Vector3 vel_;	//速度
	float dir_;		//向き

	bool isDead_;	//死亡フラグ
	int priority_;	//優先度
};

