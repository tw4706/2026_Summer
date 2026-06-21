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
	/// プレイヤーの位置の取得
	/// </summary>
	/// <returns>PlayerのPos</returns>
	Vector3 GetPlayerPos()const;

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="pPlayer">プレイヤーのポインタ</param>
	void SetPlayer(std::weak_ptr<Player>pPlayer);

private:
	//プレイヤーの弱参照
	std::weak_ptr<Player>pPlayer_;
};

