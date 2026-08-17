#pragma once
#include "Character.h"
#include "Animation.h"
#include "AStarPathFinder.h"
#include "PathFollower.h"
#include "WayPointLoader.h"
#include "EnemyAttackDataLoader.h"
#include "EnemyData.h"
#include "EnemySpawnData.h"
#include "PlayerActionCounter.h"
#include "../UI/EnemyHPGaugeUI.h"
#include "Enemynavigation.h"

class Player;
class SphereCollider;
class EnemyStateBase;
class EnemyBase :public Character
{
	friend class EnemyStateBase;
	friend class EnemyStateIdle;
	friend class EnemyStateRun;
	friend class EnemyStateReturn;
	friend class EnemyStateJump;
	friend class EnemyStateReact;
public:
	EnemyBase();
	virtual~EnemyBase();

	virtual void Init()override;
	virtual void Update()override;
	virtual void Draw()override;

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="obj">衝突したゲームオブジェクト</param>
	virtual void OnCollision(Collidable& coll, Collider* pColliderA, Collider* pColliderB)override;

	/// <summary>
	/// 体力を取得
	/// </summary>
	/// <returns>体力を返す</returns>
	int GetHP()const { return hp_; }

	/// <summary>
	/// 最大体力の取得
	/// </summary>
	/// <returns>最大体力を返す</returns>
	int GetMaxHP()const { return maxHP_; }

	/// <summary>
	/// ダメージ処理
	/// </summary>
	/// <param name="damage">ダメージ</param>
	virtual void OnDamage(int damage);

	/// <summary>
	/// 攻撃コライダーの生成
	/// </summary>
	/// <param name="radius">半径</param>
	/// <param name="distance">距離</param>
	void CreateAttackCollider(float radius, float distance, int attackDamage);

	/// <summary>
	/// 飛ばす攻撃用のコライダーの生成
	/// </summary>
	/// <param name="radius">攻撃コライダーの半径</param>
	/// <param name="speed">攻撃コライダーの進む速度</param>
	/// <param name="attackDamage">ダメージ</param>
	void CreateRangedAttackCollider(float radius, float speed, int attackDamage);

	/// <summary>
	/// 攻撃コライダーの削除
	/// </summary>
	void RemoveAttackCollider();

	/// <summary>
	/// 攻撃で与えるダメージの取得
	/// </summary>
	/// <returns>攻撃によるダメージ</returns>
	int GetAttackDamage() const { return attackDamage_; }

	/// <summary>
	/// CSVデータの適用
	/// </summary>
	/// <param name="data">CSVファイルのデータ</param>
	virtual void ApplyData(const EnemyData& data, const EnemySpawnData& spawnData, const WayPointLoader* pWayPointLoader);

	/// <summary>
	/// 攻撃データローダーのセット
	/// </summary>
	/// <param name="pLoader">攻撃データローダーのポインタ</param>
	void SetAttackDataLoader(const EnemyAttackDataLoader* pLoader) { pAttackDataLoader_ = pLoader; }

	/// <summary>
	/// 攻撃データローダーの取得
	/// </summary>
	const EnemyAttackDataLoader* GetAttackDataLoader() const { return pAttackDataLoader_; }

	/// <summary>
	/// 移動角度の取得
	/// </summary>
	float GetMoveAngle() const { return moveAngle_; }

	/// <summary>
	/// 移動角度の設定
	/// </summary>
	void SetMoveAngle(float angle) { moveAngle_ = angle; }

	/// <summary>
	/// プレイヤーの位置の取得
	/// </summary>
	/// <returns>PlayerのPos</returns>
	Vector3 GetPlayerPos()const;

	/// <summary>
	/// カメラから見た時のターゲット座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetCameraTarget()const;

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="pPlayer">プレイヤーのポインタ</param>
	void SetPlayer(std::weak_ptr<Player>pPlayer);

	/// <summary>
	/// プレイヤーの行動カウンタの取得	
	/// </summary>
	/// <returns>プレイヤーが存在しない場合はnullptr</returns>
	const PlayerActionCounter* GetPlayerActionCounter() const;

	/// <summary>
	/// 索敵範囲の取得
	/// </summary>
	/// <returns>索敵範囲</returns>
	float GetSearchRadius() const { return searchRadius_; }

	/// <summary>
	/// 攻撃コライダーの取得
	/// </summary>
	/// <returns>攻撃コライダーのポインタ</returns>
	const SphereCollider* GetAttackCollider()const { return pAttackCollider_; }

	/// <summary>
	/// ステージモデルハンドルの取得(視線判定のRayに使う)
	/// </summary>
	int GetStageModelHandle() const { return stageModelHandle_; }

	/// <summary>
	/// ステージモデルハンドルのセット
	/// </summary>
	void SetStageModelHandle(int handle) { stageModelHandle_ = handle; }

	/// <summary>
	/// スローアニメーション速度の設定
	/// </summary>
	void SetSlowAnimationSpeed();

	/// <summary>
	/// 攻撃アニメーション速度の設定
	/// </summary>
	void SetAttackAnimationSpeed();

	/// <summary>
	/// プレイヤーが範囲の中に入っているか
	/// </summary>
	/// <returns>入ってたらtrue,入ってなかったらfalse</returns>
	bool IsPlayerInRange(float radius)const;

	/// <summary>
	/// プレイヤーが視界に入っているかどうか
	/// </summary>
	bool IsPlayerInVision(float maxDist, float visionAngle) const;

	/// <summary>
	/// ボスが反応しているかどうか(反応ステートに入っているか)
	/// </summary>
	/// <returns></returns>
	bool IsReacting() const;

	/// <summary>
	/// HPUIのセット
	/// </summary>
	/// <param name="pGauge">ゲージの弱参照</param>
	void SetHPGaugeUI(std::weak_ptr<EnemyHPGaugeUI> pGauge) { pHPGaugeUI_ = pGauge; }

	/// <summary>
	/// デバッグ用の索敵範囲の描画
	/// </summary>
	/// <param name="centerPos">中心座標</param>
	/// <param name="radius">半径</param>
	/// <param name="color">色</param>
	void DrawDebugSearchRange(const Vector3& centerPos, float radius, unsigned int color);

	/// <summary>
	/// 経路探索コンポーネントの生成
	/// </summary>
	void CreateNavigation();

	/// <summary>
	/// 経路探索コンポーネントの取得
	/// </summary>
	EnemyNavigation* GetNavigation() const { return pNavigation_.get(); }

	/// <summary>
	/// ナビゲーショングリッドのセット
	/// </summary>
	void SetNavigationGrid(const NavigationGrid* pNavGrid);

	/// <summary>
	/// 雑魚敵殲滅用関数
	/// </summary>
	void Kill() { hp_ = 0; isDead_ = true; }
protected:

	/// <summary>
	/// 視野角の描画をすべきかどうか
	/// </summary>
	/// <returns>すべきでない場合はfalse,それ以外はtrue</returns>
	virtual bool ShouldDrawVisionDebug() const { return true; }

protected:
	//移動の際に向いている角度
	float moveAngle_;

	//ジャンプの着地位置の座標
	Vector3 jumpTargetPos_;

	//ステージのモデルハンドル(視線判定用)
	int stageModelHandle_ = -1;

	//最大体力
	int maxHP_ = 0;

	//攻撃力
	int attackDamage_ = 0;

	//遠距離攻撃かどうか
	bool isRangedAttack_ = false;

	//遠距離攻撃の飛んでいく方向
	Vector3 rangedAttackDir_ = { 0.0f, 0.0f, 0.0f };

	//遠距離攻撃の飛んでいく速度
	float rangedAttackSpeed_ = 0.0f;

	//モデルの拡大率
	Vector3 scale_;

	//索敵範囲
	float searchRadius_ = 0.0f;

	//コライダーの半径と高さ
	float colliderRadius_ = 0.0f;
	float colliderHeight_ = 0.0f;

	//視野角のデバッグ描画をすべきかどうか
	bool shouldDebugDrawVision_ = false;

	//攻撃コライダー
	SphereCollider* pAttackCollider_ = nullptr;
	float attackColliderDistance_ = 0.0f;

	//攻撃データローダーの参照
	const EnemyAttackDataLoader* pAttackDataLoader_ = nullptr;

	//プレイヤーの弱参照
	std::weak_ptr<Player>pPlayer_;

	//自身のHPゲージUIへの弱参照
	std::weak_ptr<EnemyHPGaugeUI> pHPGaugeUI_; 

	//経路探索のコンポーネント
	std::unique_ptr<EnemyNavigation> pNavigation_;
};

