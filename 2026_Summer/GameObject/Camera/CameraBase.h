#pragma once
#include"Math/Vector3.h"
#include"Collider/Collidable.h"

class CameraBase
{
public:
	CameraBase();
	virtual~CameraBase();

	virtual void Init()abstract;
	virtual void Update(int stageModelHandle);

	/// <summary>
	/// 計算したカメラの位置と注視点を適用する関数
	/// </summary>
	void UpdateRenderSystem();

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
	/// <returns>ヨー角(Z軸(上下)の回転角)</returns>
	virtual float GetYaw() const { return yaw_; }

	/// <summary>
	/// カメラとステージの当たり判定
	/// </summary>
	/// <param name="stageModelHandle">ステージのモデルハンドル</param>
	/// <param name="startPos">始点</param>
	/// <param name="endPos">終点</param>
	/// <returns>すてーじとあたっているかどうかを返す</returns>
	Vector3 CheckCollCameraToStage(int stageModelHandle, const Vector3& startPos, const Vector3& endPos);

	/// <summary>
	/// 強制的にカメラの座標とターゲットを設定する関数
	/// </summary>
	/// <param name="prevCameraPos">前のカメラ座標</param>
	/// <param name="prevCameraTarget">前のカメラターゲット</param>
	void SetPrevPosAndTarget(Vector3& prevCameraPos, Vector3& prevCameraTarget);
private:
	/// <summary>
	/// カメラシェイクの更新
	/// </summary>
	/// <returns>シェイク後のカメラ座標</returns>
	Vector3 UpdateShake();
protected:
	Vector3 pos_;          //座標
	Vector3 cameraTarget_; //注視点
	float fov_;            //視野角
	float fovTarget_;      //ターゲットの視野角
	float yaw_;             //ヨー角
	float pitch_;           //ピッチ角
	float currentYaw_;      //現在のヨー角
	float currentPitch_;    //現在のピッチ角

	float shakeTime_;      //シェイク時間
	float shakePower_;     //シェイクの強さ
};

