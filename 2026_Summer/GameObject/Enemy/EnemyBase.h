#pragma once
#include "Character.h"
#include "Animation.h"
#include "AStarPathFinder.h"
#include "PathFollower.h"
#include "EnemyData.h"

class Player;
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

	//A*
	AStarPathFinder pathFinder_;
	//パスフォロワー
	PathFollower pathFollower_;

	//ナビゲーショングリッド
	const NavigationGrid* pNaviGrid_ = nullptr;
};

