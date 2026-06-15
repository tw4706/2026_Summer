#pragma once
#include"Vector3.h"

class CameraBase
{
public:
	CameraBase();
	virtual~CameraBase();

	virtual void Init()abstract;
	virtual void Update()abstract;

	/// <summary>
	/// カメラの回転
	/// </summary>
	/// <param name="yaw">ヨー角</param>
	/// <param name="pitch">ピッチ角</param>
	virtual void AddRotation(float yaw, float pitch) {}

	/// <summary>
	/// カメラのシェイク
	/// </summary>
	/// <param name="time">カメラを揺らす時間</param>
	/// <param name="power">カメラを揺らす力</param>
	void Shake(float time, float power);

	/// <summary>
	/// ズーム開始関数
	/// </summary>
	/// <param name="fov">視野角</param>
	void StartZoom(float fov);

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>カメラの座標</returns>
	const Vector3& GetPos()const { return pos_; }

	/// <summary>
	/// カメラの注視点の取得
	/// </summary>
	/// <returns>カメラの注視点</returns>
	const Vector3& GetCameraTarget()const { return cameraTarget_; }

	/// <summary>
	/// ヨー角の取得
	/// </summary>
	/// <returns>ヨー角</returns>
	virtual float GetYaw() const { return 0.0f; }

protected:

	/// <summary>
	/// カメラシェイクの更新
	/// </summary>
	/// <returns>シェイク後のカメラ座標</returns>
	Vector3 UpdateShake();



	/// <summary>
	/// 派生したクラスで計算したカメラの位置と注視点をDxLibに適用させる関数
	/// </summary>
	void ApplyToDxLib();

protected:
	Vector3 pos_;          //座標
	Vector3 cameraTarget_; //注視点
	float fov_;            //視野角
	float fovTarget_;      //ターゲットの視野角

	float shakeTime_;      //シェイク時間
	float shakePower_;     //シェイクの強さ
};

