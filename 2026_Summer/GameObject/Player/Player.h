#pragma once
#include "Character.h"
#include "Model.h"
#include "Animation.h"

class Input;
class Katana;
class PlayerCamera;
class CharacterStateBase;
class Player :public Character
{
	friend class PlayerStateBase;
	friend class PlayerStateAttack;
	friend class PlayerStateRun;
	friend class PlayerStateDodge;
	friend class PlayerStateDamage;
	friend class PlayerStateJump;
	friend class PlayerStateIdle;
	friend class PlayerStateGuard;
public:
	Player();
	virtual~Player();

	void Init()override;
	void Update()override;
	void Draw()override;

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="obj">衝突したゲームオブジェクト</param>
	void OnCollision(Collidable& coll)override;

	/// <summary>
	/// 被ダメージ処理
	/// </summary>
	/// <param name="damage"></param>
	void OnDamage(const int damage);

	/// ==============================
	/// ゲッター・セッター
	/// ==============================
	/// <summary>
	/// カメラの注視点の取得
	/// </summary>
	/// <returns>カメラの注視点位置</returns>
	Vector3 GetCameraTarget()const;

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void SetCamera(PlayerCamera* camera) { pCamera_ = camera; }

	/// <summary>
	/// 刀の当たり判定の有効/無効を設定する
	/// </summary>
	void SetKatanaColliderEnabled(bool isEnabled);

	/// <summary>
	/// 刀のポインタを取得
	/// </summary>
	/// <returns>刀のポインタ</returns>
	Katana* GetKatana() const { return pKatana_.get(); }

	/// <summary>
	/// 死んだときに呼ばれる処理
	/// </summary>
	void Death() { isDead_ = true; }

private:
	float moveAngle_;						//プレイヤーの向く角度
	int handFrameIndex_;					//右手の刀を持つ手のフレームインデックス
	float jumpPower_ = 18.0f;				//ジャンプ力
	bool isInvincible_ = false;				//無敵中かどうか
	bool isGuarding_ = false;				//ガード中かどうか

	PlayerCamera* pCamera_ = nullptr;		//カメラ
	std::unique_ptr<Katana> pKatana_;		//刀
};

