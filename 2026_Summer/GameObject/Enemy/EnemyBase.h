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

	virtual void Init()override {};
	virtual void Update()override {};
	virtual void Draw()override {};

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="obj">衝突したゲームオブジェクト</param>
	void OnCollision(GameObject* obj)override {};

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

	float GetMoveAngle() const { return moveAngle_; }
	void SetMoveAngle(float angle) { moveAngle_ = angle; }

protected:
	//向いている角度
	float moveAngle_;

	//プレイヤーの弱参照
	std::weak_ptr<Player>pPlayer_;
};

