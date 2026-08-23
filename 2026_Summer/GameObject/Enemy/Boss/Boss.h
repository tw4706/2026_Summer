#pragma once
#include "Enemy/EnemyBase.h"
#include "Katana.h"
class Boss :public EnemyBase
{
public:
	Boss();
	virtual~Boss();

	virtual void Init()override;
	virtual void Update()override;
	virtual void Draw()override;

	/// <summary>
	/// 攻撃の準備ができたかどうか
	/// </summary>
	/// <returns></returns>
	bool IsAttackReady()const;

	/// <summary>
	/// 攻撃のクールダウン
	/// </summary>
	void AttackCoolDown();

	/// <summary>
	/// ダー目地処理
	/// </summary>
	/// <param name="damage">ダメージ</param>
	void OnDamage(int damage)override;

	void OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)override;

	/// <summary>
	/// 攻撃コライダーの座標の取得
	/// </summary>
	/// <returns>攻撃コライダあーの座標を返す</returns>
	Vector3 GetAttackColliderPos() const;

	/// <summary>
	/// 遠距離攻撃コライダーの進行方向の角度を取得
	/// </summary>
	/// <returns></returns>
	float GetRangedAttackAngle() const;

protected:
	/// <summary>
	/// 視野角の描画をすべきかどうか
	/// </summary>
	/// <returns>すべきでない場合はfalse,それ以外はtrue</returns>
	bool ShouldDrawVisionDebug() const override { return false; }
private:
	float attackCoolTime_ = 0.0f;//攻撃のクールタイム

	//刀
	std::unique_ptr<Katana> pKatana_;
	int handFrameIndex_ = -1;
};

