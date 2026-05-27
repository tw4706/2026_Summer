#pragma once
#include "GameObject.h"

class Input;
class Camera;
class Player :public GameObject
{
public:
	Player();
	~Player();

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
private:

	float moveAngle_;						//プレイヤーの向く角度
	Camera* pCamera_ = nullptr;				//カメラ

	int model_;								//モデル
};

