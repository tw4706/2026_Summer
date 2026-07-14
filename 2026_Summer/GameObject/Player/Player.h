#pragma once
#include "Character.h"
#include "Model.h"
#include "Animation.h"
#include "Camera/CameraManager.h"
#include "../../PlayerActionCounter.h"

class Input;
class Katana;
class EnemyBase;
class CameraBase;
class CharacterStateBase;
class Player :public Character
{
	friend class PlayerStateBase;
	friend class PlayerStateAttack;
	friend class PlayerStateRun;
	friend class PlayerStateDodge;
	friend class PlayerStateDamage;
	friend class PlayerStateJump;
	friend class PlayerStateIdle;
	friend class PlayerStateGuard;
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
	void OnCollision(Collidable& coll)override;

	/// <summary>
	/// 被ダメージ処理
	/// </summary>
	/// <param name="damage"></param>
	void OnDamage(const int damage);

	/// ==============================
	/// ゲッター・セッター
	/// ==============================
	
	/// <summary>
	/// 正面ベクトルの取得
	/// </summary>
	/// <returns>正面ベクトルを返す</returns>
	Vector3 GetForWardVec()const { return Vector3{ std::sinf(moveAngle_), 0.0f, -std::cosf(moveAngle_) }; }

	/// <summary>
	/// カメラの注視点の取得
	/// </summary>
	/// <returns>カメラの注視点位置</returns>
	Vector3 GetCameraTarget()const;

	/// <summary>
	/// カメラマネージャーからアクティなカメラを取得する
	/// </summary>
	/// <returns>アクティブなカメラを返す</returns>
	CameraBase* GetActiveCamera() const { return pCameraManager_ ? pCameraManager_->GetActiveCamera().get() : nullptr; }

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void SetCameraManager(CameraManager* manager) { pCameraManager_ = manager; }

	/// <summary>
	/// 移動角度の設定
	/// </summary>
	/// <param name="angle">角度</param>
	void SetMoveAngle(float angle) { moveAngle_ = angle; }

	/// <summary>
	/// 刀の当たり判定の有効/無効を設定する
	/// </summary>
	void SetKatanaColliderEnabled(bool isEnabled);

	/// <summary>
	/// 刀のポインタを取得
	/// </summary>
	/// <returns>刀のポインタ</returns>
	Katana* GetKatana() const { return pKatana_.get(); }

	/// <summary>
	/// ロックオン中かどうかの取得
	/// </summary>
	bool IsLockOn() const { return isLockOn_; }

	/// <summary>
	/// ロックオン状態の設定
	/// </summary>
	void SetLockOn(bool isLockOn) { isLockOn_ = isLockOn; }

	/// <summary>
	/// ロックオン対象の敵の取得
	/// </summary>
	std::weak_ptr<EnemyBase> GetLockOnEnemy() const { return pLockOnEnemy_; }

	/// <summary>
	/// ロックオン対象の敵の設定
	/// </summary>
	void SetLockOnEnemy(std::weak_ptr<EnemyBase> enemy) { pLockOnEnemy_ = enemy; }

	/// <summary>
	/// プレイヤーの行動カウンタの取得
	/// </summary>
	const PlayerActionCounter& GetActionCounter() const { return actionCounter_; }

private:
	float moveAngle_;						//プレイヤーの向く角度
	int handFrameIndex_;					//右手の刀を持つ手のフレームインデックス
	float jumpPower_ = 18.0f;				//ジャンプ力
	bool isInvincible_ = false;				//無敵中かどうか
	bool isGuarding_ = false;				//ガード中かどうか
	bool isLockOn_ = false;					//ロックオンしているかどうか
	int hpUIHandle_;						//HPUIハンドル
	int hpUIFrameHandle_;					//HPバーUIハンドル
	int hpUIX_, hpUIY_;						//グラフィックハンドルのサイズ取得用
	int hpBarUIX_, hpBarUIY_;				//グラフィックハンドルのサイズ取得用
	int reticleUIHandle_;					//レティクルUIハンドル

	CameraManager* pCameraManager_=nullptr;			//カメラマネージャー
	CameraBase* activeCamera_ = nullptr;	//アクティブなカメラ
	std::weak_ptr<EnemyBase>pLockOnEnemy_;	//敵のポインタ
	std::unique_ptr<Katana> pKatana_;		//刀
	PlayerActionCounter actionCounter_; //プレイヤーの行動傾向カウンタ
};

