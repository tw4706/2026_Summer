#pragma once
#include "GameObject.h"
#include "Animation.h"

class Input;
class Camera;
class Player :public GameObject
{
public:
	//プレイヤーの状態
	enum class PlayerState
	{
		Idle,	//待機
		Run,	//移動
		Jump,	//ジャンプ
		Attack,	//攻撃
	};

	Player();
	virtual~Player();

	void Init()override;
	void Update()override {};
	void Update(Input& input);
	void Draw()override;

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

private:

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="input">入力</param>
	void Move(Input& input);

	/// <summary>
	/// ジャンプ
	/// </summary>
	/// <param name="input">入力</param>
	void Jump(Input& input);

	/// <summary>
	/// 攻撃
	/// </summary>
	/// <param name="input">入力</param>
	void Attack(Input& input);

	/// <summary>
	/// アナログスティックの更新
	/// </summary>
	/// <param name="input">入力</param>
	void UpdateAnalogStick(Input& input);

	/// <summary>
	/// 状態遷移の更新
	/// </summary>
	void UpdateState();

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	void UpdateAnimation(float dt);

private:
	int modelH_;							//プレイヤーのモデル
	int katanaH_;							//刀のモデル
	float moveAngle_;						//プレイヤーの向く角度
	int handFrameIndex_;					//右手の刀を持つ手のフレームインデックス

	float gravity_ = 0.5f;					//重力の強さ
	float jumpPower_ = 12.0f;				//ジャンプ力
	bool isGround_ = true;					//接地フラグ
	bool isAttack_ = false;					//攻撃フラグ

	Camera* pCamera_ = nullptr;				//カメラ
	Animation animation_;					//アニメーション
	PlayerState state_;						//プレイヤーの状態
};

