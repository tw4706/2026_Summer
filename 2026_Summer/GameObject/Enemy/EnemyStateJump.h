#pragma once
#include "EnemyStateBase.h"
#include "Math/Vector3.h"

class EnemyStateJump :public EnemyStateBase
{
public:
	EnemyStateJump(std::weak_ptr<EnemyBase> pEnemy, float searchRadius);
	~EnemyStateJump() = default;

	virtual void Enter()  override;
	virtual void Update() override;
	virtual void Exit()   override;

	/// <summary>
	/// ジャンプの座標の設定
	/// </summary>
	/// <param name="startPos">ジャンプの開始座標</param>
	/// <param name="targetPos">着地地点の座標</param>
	void SetUpJumpPos(const Vector3& startPos,const Vector3& targetPos);
private:
	float deltaTime_ = 0.0f;	//経過時間
	float velocityY_ = 0.0f;	//Y軸の現在の速度

	Vector3 jumpStartPos_;		//ジャンプのスタート位置
	Vector3 jumpEndPos_;		//ジャンプの終了位置
	Vector3 moveDir_;			//XZの移動方向
};

