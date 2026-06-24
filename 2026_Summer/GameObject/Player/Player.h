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
	void OnCollision(GameObject* obj)override;

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

	void SetInput(Input* input) { pInput_ = input; }

	float GetMoveAngle() const { return moveAngle_; }
	void SetMoveAngle(float angle) { moveAngle_ = angle; }

	bool GetIsInvincible() const { return isInvincible_; }
	void SetIsInvincible(bool isInvincible) { isInvincible_ = isInvincible; }

	float GetJumpPower() const { return jumpPower_; }

	/// <summary>
	/// 刀の当たり判定の有効/無効を設定する
	/// </summary>
	void SetKatanaColliderEnabled(bool isEnabled);

private:
	float moveAngle_;						//プレイヤーの向く角度
	int handFrameIndex_;					//右手の刀を持つ手のフレームインデックス
	float jumpPower_ = 12.0f;				//ジャンプ力
	bool isInvincible_ = false;				//無敵中かどうか

	PlayerCamera* pCamera_ = nullptr;		//カメラ
	Input* pInput_ = nullptr;				//インプット入力
	std::unique_ptr<Katana> pKatana_;		//刀
};

