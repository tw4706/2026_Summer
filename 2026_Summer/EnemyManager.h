#pragma once
#include "EnemyStatus.h"
#include "EnemySpawnDataLoader.h"
#include "WayPointLoader.h"
#include <memory>
#include <vector>
#include <string>

class EnemyBase;
class Player;
class NavigationGrid;
class EnemyManager
{
public:
	EnemyManager();
	virtual~EnemyManager();

	void Update();
	void Draw();

	/// <summary>
	/// 敵データを読み込む
	/// </summary>
	/// <param name="path">敵データファイルの文字列の参照</param>
	/// <returns>読み込みに成功した場合はtrue、失敗した場合はfalseを返す</returns>
	bool LoadEnemyData(const std::wstring& path);

	/// <summary>
	/// 敵データを読み込む
	/// </summary>
	/// <param name="path">敵データファイルの文字列の参照</param>
	/// <returns>読み込みに成功した場合はtrue、失敗した場合はfalseを返す</returns>
	bool LoadEnemySpawnData(const std::wstring& path);

	/// <summary>
	/// WayPointデータを読み込む
	/// </summary>
	/// <param name="path">WayPointデータファイルの文字列の参照</param>
	/// <returns>読み込みに成功した場合はtrue、失敗した場合はfalseを返す</returns>
	bool LoadWayPointData(const std::wstring& path);

	/// <summary>
	/// 指定されたパス種別に応じた敵キャラクターの生成
	/// </summary>
	/// <param name="pathtype">CSVのType列に対応する文字列</param>
	/// <returns>生成された敵キャラクター,失敗したらnullptr</returns>
	std::vector<std::shared_ptr<EnemyBase>>SpawnEnemyArea(int areaId);

	/// <summary>
	/// プレイヤーへの参照を全敵にセット
	/// </summary>
	void SetPlayer(std::weak_ptr<Player> pPlayer);

	/// <summary>
	/// ナビゲーショングリッドを全敵にセット
	/// </summary>
	void SetNavigationGrid(const NavigationGrid* pNavGrid);

	/// <summary>
	/// 死亡済みなど不要な敵を削除する
	/// </summary>
	void RemoveEnemy();

	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemies() const { return enemies_; }

private:
	/// <summary>
	/// 型名に応じた敵のインスタンスを生成する
	/// </summary>
	/// <param name="type">生成する敵の型を表す文字列</param>
	/// <returns>生成されたEnemyBaseオブジェクトのスマートポインタ</returns>
	std::shared_ptr<EnemyBase> CreateInstance(const std::string& type);
private:
	//生成中の敵一覧
	std::vector<std::shared_ptr<EnemyBase>> enemies_;

	//敵のパラメータのデータローダー
	EnemyStatus dataLoader_;

	//敵の生成データのローダー
	EnemySpawnDataLoader spawnDataLoader_;

	//WayPointのデータローダー
	WayPointLoader wayPointLoader_;
};

