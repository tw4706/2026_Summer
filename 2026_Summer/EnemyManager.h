#pragma once
#include "EnemyStatus.h"
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
	/// パスから敵データを読み込む
	/// </summary>
	/// <param name="path">敵データファイルのパスのワイド文字列の参照</param>
	/// <returns>データの読み込みに成功した場合はtrue、失敗した場合はfalseを返す。</returns>
	bool LoadEnemyData(const std::wstring& path);

	/// <summary>
	/// 指定されたパス種別に応じた敵キャラクターの生成
	/// </summary>
	/// <param name="pathtype">CSVのType列に対応する文字列</param>
	/// <returns>生成された敵キャラクター,失敗したらnullptr</returns>
	std::shared_ptr<EnemyBase>SpawnEnemy(const std::string& pathType);

	/// <summary>
	/// プレイヤーへの参照を全敵にセット
	/// </summary>
	void SetPlayer(std::weak_ptr<Player> pPlayer);

	/// <summary>
	/// ナビゲーショングリッドを全敵にセット
	/// </summary>
	void SetNavigationGrid(const NavigationGrid* pNavGrid);

	/// <summary>
	/// 死亡済みなど不要な敵を破棄したい場合に呼ぶ
	/// </summary>
	void RemoveEnemy();

	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemies() const { return enemies_; }

private:

	/// <summary>
	/// 指定された型名に応じたEnemyBaseのインスタンスを生成する
	/// </summary>
	/// <param name="type">生成するエネミーの型名を表す文字列</param>
	/// <returns>生成されたEnemyBaseオブジェクトのスマートポインタ</returns>
	std::shared_ptr<EnemyBase> CreateInstance(const std::string& type);


private:
	//生成中の敵一覧
	std::vector<std::shared_ptr<EnemyBase>> enemies_;

	//CSVのデータロード
	EnemyStatus dataLoader_;
};

