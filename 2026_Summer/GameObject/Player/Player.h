#pragma once
#include "Character.h"
#include "Model.h"
#include "Animation.h"
#include "Camera/CameraManager.h"
#include "ComboManager.h"
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
	void OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)override;

	/// <summary>
	/// 被ダメージ処理
	/// </summary>
	/// <param name="damage"></param>
	void OnDamage(const int damage);

	/// ==============================
	/// ゲッター・セッター
	/// ==============================
	
	/// <summary>
	/// 現在のHPの取得
	/// </summary>
	/// <returns></returns>
	int GetHP()const { return hp_; }

	/// <summary>
	/// 最大体力の取得
	/// </summary>
	/// <returns>最大体力を返す</returns>
	int GetMaxHP()const;

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
	/// ロックオンしている敵の方向に向く
	/// </summary>
	void LooktoLockOnEnemy();

	/// <summary>
	/// プレイヤーの行動カウンタの取得
	/// </summary>
	const PlayerActionCounter& GetActionCounter() const { return actionCounter_; }

	/// <summary>
	/// コンボマネージャーの取得
	/// </summary>
	/// <returns></returns>
	ComboManager& GetComboManager() { return comboManager_; }

	/// <summary>
	/// 刀エフェクトの再生
	/// </summary>
	void PlayKatanaEffect();

	/// <summary>
	/// 刀エフェクトの停止
	/// </summary>
	void StopKatanaEffect();

	/// <summary>
	/// アニメーションの現在の時間を取得
	/// </summary>
	/// <returns>アニメーションの現在時間を返す</returns>
	float GetAnimationCurrentTime() const;

	/// <summary>
	/// 操作可能かのフラグの取得
	/// </summary>
	/// <returns>可能ならtrueを返す、不可能ならfalseを返す</returns>
	bool GetCanControl()const { return canControl_; }

	/// <summary>
	/// 操作可能かどうかの設定
	/// </summary>
	/// <param name="canControl">操作可能か</param>
	void SetCanControl(bool canControl) { canControl_ = canControl; }

private:
	float moveAngle_;						//プレイヤーの向く角度
	int handFrameIndex_;					//右手の刀を持つ手のフレームインデックス
	float jumpPower_ = 18.0f;				//ジャンプ力
	bool isInvincible_ = false;				//無敵中かどうか
	bool isGuarding_ = false;				//ガード中かどうか
	bool isLockOn_ = false;					//ロックオンしているかどうか
	bool canControl_ = true;				//操作可能かどうか
	bool isPlayedGuardSE_ = false;			//ガードSEをさいせいしたかどうか			

	ComboManager comboManager_;				//コンボマネージャー
	CameraManager* pCameraManager_=nullptr;	//カメラマネージャー
	CameraBase* activeCamera_ = nullptr;	//アクティブなカメラ
	std::weak_ptr<EnemyBase>pLockOnEnemy_;	//敵のポインタ
	std::unique_ptr<Katana> pKatana_;		//刀
	PlayerActionCounter actionCounter_;		//プレイヤーの行動傾向カウンタ
};