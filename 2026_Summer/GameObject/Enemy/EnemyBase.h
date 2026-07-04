#pragma once
#include "Character.h"
#include "Animation.h"
#include "AStarPathFinder.h"
#include "PathFollower.h"
#include "WayPointLoader.h"
#include "EnemyData.h"

class Player;
class SphereCollider;
class EnemyStateBase;
class EnemyBase :public Character
{
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
	virtual void ApplyData(const EnemyData& data);

	/// <summary>
	/// プレイヤーの位置の取得
	/// </summary>
	/// <returns>PlayerのPos</returns>
	Vector3 GetPlayerPos()const;

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="pPlayer">プレイヤーのポインタ</param>
	void SetPlayer(std::weak_ptr<Player>pPlayer);

	/// <summary>
	/// 移動時の角度の取得
	/// </summary>
	/// <returns>角度を返す</returns>
	float GetMoveAngle() const { return moveAngle_; }

	/// <summary>
	/// 移動時の角度の設定
	/// </summary>
	/// <param name="angle">角度</param>
	void SetMoveAngle(float angle) { moveAngle_ = angle; }

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
	/// A*の取得
	/// </summary>
	/// <returns>A*の参照を返す</returns>
	AStarPathFinder& GetPathFinder() { return pathFinder_; }

	/// <summary>
	/// パスフォロワーオブジェクトへの参照を取得
	/// </summary>
	/// <returns>pathFollower_の参照を返す</returns>
	PathFollower& GetPathFollower() { return pathFollower_; }

	/// <summary>
	/// ナビゲーショングリッドの設定
	/// </summary>
	/// <param name="pNavGrid">設定するナビゲーショングリッドのポインタ</param>
	void SetNavigationGrid(const NavigationGrid* pNavGrid);


	/// <summary>
	/// エリアIDの取得
	/// </summary>
	/// <returns></returns>
	int GetAreaId()const { return areaId_; }

	/// <summary>
	/// エリアIDのセット
	/// </summary>
	/// <param name="areaId"></param>
	void SetAreaId(int areaId) { areaId_ = areaId; }

	/// <summary>
	/// WayPointLoaderの取得
	/// </summary>
	/// <returns>WayPointLoaderのポインタ</returns>
	const WayPointLoader* GetWayPointLoader() const { return pWayPointLoader_; }

	/// <summary>
	/// WayPointLoaderの設定
	/// </summary>
	/// <param name="pWayPointLoader">設定するWayPointLoaderのポインタ</param>
	void SetWayPointLoader(const WayPointLoader* pWayPointLoader) { pWayPointLoader_ = pWayPointLoader; }

	/// <summary>
	/// 現在いるWayPointのIDを取得
	/// </summary>
	/// <returns>現在のWayPointID</returns>
	int GetCurrentWayPointId() const { return currentWayPointId_; }

	/// <summary>
	/// 現在いるWayPointのIDを設定
	/// </summary>
	/// <param name="id">WayPointID</param>
	void SetCurrentWayPointId(int id) { currentWayPointId_ = id; }

	/// <summary>
	/// 目標のWayPointのIDを取得
	/// </summary>
	/// <returns>目標のWayPointID</returns>
	int GetNextWayPointId() const { return nextWayPointId_; }

	/// <summary>
	/// 目標のWayPointのIDを設定
	/// </summary>
	/// <param name="id">WayPointID</param>
	void SetNextWayPointId(int id) { nextWayPointId_ = id; }

	/// <summary>
	/// デバッグ描画の座標をセット
	/// </summary>
	/// <param name="pos"></param>
	void SetDebugNextPos(const Vector3& pos) { debugNextPos_ = pos; }
	void SetHasDebugTarget(bool has) { hasDebugTarget_ = has; }

protected:

	//移動の際に向いている角度
	float moveAngle_;

	//ステージのモデルハンドル(視線判定用)
	int stageModelHandle_ = -1; 

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
	//A*
	AStarPathFinder pathFinder_;
	//パスフォロワー
	PathFollower pathFollower_;
	//ナビゲーショングリッド
	const NavigationGrid* pNaviGrid_ = nullptr;
	//エリアID
	int areaId_ = 0;
	//WayPointLoaderの参照
	const WayPointLoader* pWayPointLoader_ = nullptr;
	//現在いるWayPointのID
	int currentWayPointId_ = -1;
	//目標のWayPointのID
	int nextWayPointId_ = -1;

	//デバッグ描画用の目標までの座標
	Vector3 debugNextPos_ = { 0.0f, 0.0f, 0.0f };
	bool hasDebugTarget_ = false;

	//攻撃コライダー
	SphereCollider*pAttackCollider_;
	float attackColliderDistance_ = 0.0f;
};

