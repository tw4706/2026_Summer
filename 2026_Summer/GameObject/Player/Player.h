#pragma once
#include "Character.h"
#include "Animation.h"

class Input;
class Camera;
class PlayerStateBase;
class Player :public Character
{
public:
	Player();
	virtual~Player();

	void Init()override;
	void Update()override;
	void Draw()override;

	void ChangeState(std::shared_ptr<PlayerStateBase> nextState);
	void ChangeAnimation(AnimationState state);

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
	void SetCamera(Camera* camera) { pCamera_ = camera; }

	void SetInput(Input* input) { pInput_ = input; }

	Vector3 GetPosition() const { return pos_; }
	void AddPosition(const Vector3& offset);

	Vector3 GetVelocity() const { return vel_; }
	void SetVelocity(const Vector3& vel) { vel_ = vel; }

	float GetMoveAngle() const { return moveAngle_; }
	void SetMoveAngle(float angle) { moveAngle_ = angle; }

	bool GetIsGround() const { return isGround_; }
	void SetIsGround(bool isGround) { isGround_ = isGround; }

	bool GetIsInvincible() const { return isInvincible_; }
	void SetIsInvincible(bool isInvincible) { isInvincible_ = isInvincible; }

	float GetJumpPower() const { return jumpPower_; }
	float GetGravity() const { return gravity_; }
	bool IsAnimationEnd() const { return animation_.IsEnd(); }

private:
	int modelH_;							//プレイヤーのモデル
	int katanaH_;							//刀のモデル
	float moveAngle_;						//プレイヤーの向く角度
	int handFrameIndex_;					//右手の刀を持つ手のフレームインデックス

	float gravity_ = 0.5f;					//重力の強さ
	float jumpPower_ = 12.0f;				//ジャンプ力
	bool isGround_ = true;					//地面についているかどうか
	bool isInvincible_ = false;				//無敵中かどうか

	Camera* pCamera_ = nullptr;				//カメラ
	Input* pInput_ = nullptr;				//インプット入力
	Animation animation_;					//アニメーション

	std::shared_ptr<PlayerStateBase> pCurrentState_ = nullptr;//現在のステートを管理するポインタ
};

