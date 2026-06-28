#pragma once
#include "Character.h"
#include "Animation.h"

class Player;
class EnemyStateBase;
class EnemyBase :public Character
{
public:
	EnemyBase();
	virtual~EnemyBase();

	virtual void Init()override;
	virtual void Update()override;
	virtual void Draw()override;

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="obj">衝突したゲームオブジェクト</param>
	void OnCollision(Collidable& coll)override;

	void OnDamage(int damage);

	/// <summary>
	/// プレイヤーの位置の取得
	/// </summary>
	/// <returns>PlayerのPos</returns>
	Vector3 GetPlayerPos()const;

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="pPlayer">プレイヤーのポインタ</param>
	void SetPlayer(std::weak_ptr<Player>pPlayer);

	/// <summary>
	/// 移動時の角度の取得
	/// </summary>
	/// <returns>角度を返す</returns>
	float GetMoveAngle() const { return moveAngle_; }

	/// <summary>
	/// 移動時の角度の設定
	/// </summary>
	/// <param name="angle">角度</param>
	void SetMoveAngle(float angle) { moveAngle_ = angle; }

protected:
	//移動の際に向いている角度
	float moveAngle_;

	//モデルの拡大率
	Vector3 scale_;

	//プレイヤーの弱参照
	std::weak_ptr<Player>pPlayer_;
};

