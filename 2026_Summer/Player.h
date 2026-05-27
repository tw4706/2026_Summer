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
	};

	Player();
	virtual~Player();

	void Init()override;
	void Update()override {};
	void Update(Input&input);
	void Draw()override;

	/// <summary>
	/// カメラの注視点の取得
	/// </summary>
	/// <returns>カメラの注視点位置</returns>
	Vector3 GetCameraTarget()const;

private:

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="input">入力</param>
	void Move(Input& input);

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
	int modelH_;								//モデル
	float moveAngle_;						//プレイヤーの向く角度

	Camera* pCamera_ = nullptr;				//カメラ
	Animation animation_;					//アニメーション
	PlayerState state_;						//プレイヤーの状態
};

