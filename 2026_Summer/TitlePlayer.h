#pragma once
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Model.h"
#include "Animation.h"
#include<memory>

class TitlePlayer
{
public:
	TitlePlayer();
	~TitlePlayer();

	void Init();
	void Update();
	void Draw();

	const Vector3& GetPos() const { return pos_; }
	void SetPos(const Vector3& pos) { pos_ = pos; }
	void SetAngle(float angle) { angle_ = angle; }

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// 現在のアニメーションの時間の取得
	/// </summary>
	/// <returns>現在のアニメーションの時間を返す</returns>
	float GetCurrentAnimTime()const { animation_.GetCurrentAnimTime(); }

	/// <summary>
	/// アニメーションが終了しているかどうか
	/// </summary>
	/// <returns>終了していたらtrue,していないならfalse</returns>
	bool IsAnimationEnd() const { animation_.IsEnd(); }

	/// <summary>
	/// アニメーションの切り替え
	/// </summary>
	/// <param name="state">アニメーションの状態</param>
	void ChangeAnimation(AnimationState state);

	/// <summary>
	/// 走行開始
	/// </summary>
	/// <param name="dir">進行方向</param>
	/// <param name="speed">移動速度</param>
	void StartRun(const Vector3& dir, float speed);

	/// <summary>
	/// 走行の停止
	/// </summary>
	void StopRun();

	//外部から進行方向を参照したい場合用
	const Vector3& GetMoveDir() const { return moveDir_; }

	Vector3 GetForward() const;

private:
	//モデル
	Model model_;
	//アニメーション
	Animation animation_;

	//座標
	Vector3 pos_;
	//拡大率
	Vector3 scale_;
	//角度
	float angle_;
	//移動方向
	Vector3 moveDir_;
	//移動速度
	float moveSpeed_;
	//走行中かどうか
	bool isRunning_;
};

