#pragma once
#include"GameObject.h"
#include<memory>

class Player;
class Camera :public GameObject
{
public:
	Camera();
	virtual~Camera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override {};

	/// <summary>
	/// カメラの回転の加算
	/// </summary>
	/// <param name="yaw">ヨー角の加算</param>
	/// <param name="pitch">ピッチの加算</param>
	void AddRotation(float yaw, float pitch);

	/// <summary>
	/// シェイク開始
	/// </summary>
	/// <param name="time">シェイク時間</param>
	/// <param name="power">加える力</param>
	void Shake(float time, float power);

	/// <summary>
	/// ズームの開始
	/// </summary>
	/// <param name="scale">ズームスケール</param>
	void StartZoom(float scale);

	/// <summary>
	/// シェイク更新
	/// </summary>
	/// <returns>更新後のカメラの位置</returns>
	Vector3 UpdateShake();

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>カメラの座標</returns>
	Vector3 GetPos()const { return pos_; }

	/// <summary>
	/// カメラの注視点の取得
	/// </summary>
	/// <returns>カメラの注視点の座標</returns>
	float GetYaw() const { return yaw_; }

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }
private:

	/// <summary>
	/// カメラの更新処理
	/// </summary>
	void UpdateCamera();

private:
	float shakeTime_;
	float shakePower_;
	float fovTarget_;
	float fov_;
	float yaw_;
	float pitch_;

	//カメラの注視点
	Vector3 cameraTarget_;
	std::shared_ptr<Player>pPlayer_;
};

