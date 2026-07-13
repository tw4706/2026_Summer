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

class Player;
class SphereCollider;
class EnemyStateBase;
class EnemyBase :public Character
{
	friend class EnemyStateBase;
	friend class EnemyStateIdle;
	friend class EnemyStateRun;
	friend class EnemyStateReturn;
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
	void OnCollision(Collidable& coll)override;

	/// <summary>
	/// ダメージ処理
	/// </summary>
	/// <param name="damage">ダメージ</param>
	void OnDamage(int damage);

	/// <summary>
	/// 攻撃コライダーの生成
	/// </summary>
	/// <param name="radius">半径</param>
	/// <param name="distance">距離</param>
	void CreateAttackCollider(float radius, float distance);

	/// <summary>
	/// 攻撃コライダーの削除
	/// </summary>
	void RemoveAttackCollider();

	/// <summary>
	/// CSVデータの適用
	/// </summary>
	/// <param name="data">CSVファイルのデータ</param>
	virtual void ApplyData(const EnemyData& data,const EnemySpawnData&spawnData, const WayPointLoader* pWayPointLoader);

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
	/// ナビゲーショングリッドの設定
	/// </summary>
	/// <param name="pNavGrid">設定するナビゲーショングリッドのポインタ</param>
	void SetNavigationGrid(const NavigationGrid* pNavGrid);

	/// <summary>
	/// スローアニメーション速度の設定
	/// </summary>
	void SetSlowAnimationSpeed();

	/// <summary>
	/// 攻撃アニメーション速度の設定
	/// </summary>
	void SetAttackAnimationSpeed();

protected:
	//移動の際に向いている角度
	float moveAngle_;

	//ステージのモデルハンドル(視線判定用)
	int stageModelHandle_ = -1;

	//HPバーのハンドル
	int hpHandle_;

	//HPバーフレームハンドル
	int hpFrameHandle_;

	//グラフィックハンドルのサイズ取得用
	int hpUIX_, hpUIY_;
	int hpBarUIX_, hpBarUIY_;

	//最大体力
	int maxHP_;

	//描画用体力
	float drawHP_;

	//HPを描画していいかどうか
	bool isDrawHPVisible_;

	int drawHPVisibleTimer_;

	//モデルの拡大率
	Vector3 scale_;

	//索敵範囲
	float searchRadius_ = 0.0f;

	//コライダーの半径と高さ
	float colliderRadius_ = 0.0f;
	float colliderHeight_ = 0.0f;

	//プレイヤーの弱参照
	std::weak_ptr<Player>pPlayer_;

	///経路探索のA*アルゴリズムとwaypointの管理
	AStarPathFinder pathFinder_;
	//パスフォロワー
	PathFollower pathFollower_;
	//ナビゲーショングリッド
	const NavigationGrid* pNaviGrid_ = nullptr;
	//エリアID
	int areaId_ = 0;
	//現在いるWayPointのID
	int currentWayPointId_ = -1;
	//目標のWayPointのID
	int nextWayPointId_ = -1;

	//デバッグ描画用の目標までの座標
	Vector3 debugNextPos_ = { 0.0f, 0.0f, 0.0f };
	bool hasDebugTarget_ = false;

	//攻撃コライダー
	SphereCollider* pAttackCollider_;
	float attackColliderDistance_ = 0.0f;

	//WayPointLoaderの参照
	const WayPointLoader* pWayPointLoader_ = nullptr;

	//攻撃データローダーの参照
	const EnemyAttackDataLoader* pAttackDataLoader_ = nullptr;
};

