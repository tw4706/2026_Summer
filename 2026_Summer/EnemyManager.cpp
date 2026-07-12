#include "EnemyManager.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/Oni.h"
#include "Enemy/BigMan.h"
#include <algorithm>

EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Update()
{
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}

bool EnemyManager::LoadEnemyData(const std::wstring& path)
{
	return dataLoader_.Load(path);
}

bool EnemyManager::LoadEnemySpawnData(const std::wstring& path)
{
	return spawnDataLoader_.Load(path);
}

bool EnemyManager::LoadWayPointData(const std::wstring& path)
{
	return wayPointLoader_.Load(path);
}

bool EnemyManager::LoadEnemyAttackData(const std::wstring& path)
{
	return attackDataLoader_.Load(path);
}

std::vector<std::shared_ptr<EnemyBase>> EnemyManager::SpawnEnemyArea(int areaId)
{
	std::vector<std::shared_ptr<EnemyBase>> spawned;

	//指定エリアの配置データを全て取得(複数体分)
	auto spawnList = spawnDataLoader_.GetSpawnDataByArea(areaId);

	for (const auto* pSpawn : spawnList)
	{
		//配置データのType名からデータを取得
		const EnemyData* pData = dataLoader_.GetEnemyData(pSpawn->type_);

		//データが取得できない場合はスキップ
		if (!pData)
		{
			continue;
		}

		//敵の種類に応じたインスタンスの作成
		auto enemy = CreateInstance(pSpawn->type_);

		//敵のインスタンスが作成できない場合はスキップ
		if (!enemy)
		{
			continue;
		}

		//パラメータ + 配置データの両方を適用
		enemy->ApplyData(*pData, *pSpawn, &wayPointLoader_);
		//攻撃データの適用
		enemy->SetAttackDataLoader(&attackDataLoader_);
		//敵の初期化
		enemy->Init();

		enemies_.push_back(enemy);
		spawned.push_back(enemy);
	}

	return spawned;
}

void EnemyManager::SetPlayer(std::weak_ptr<Player> pPlayer)
{
	//全敵にプレイヤーのセット
	for (auto& enemy : enemies_)
	{
		enemy->SetPlayer(pPlayer);
	}
}

void EnemyManager::SetNavigationGrid(const NavigationGrid* pNavGrid)
{
	//経路探索のグリッドのセット
	for (auto& enemy : enemies_)
	{
		enemy->SetNavigationGrid(pNavGrid);
	}
}

void EnemyManager::RemoveEnemy()
{
	//死亡している敵を削除する
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::shared_ptr<EnemyBase>& e) {return e->IsDead(); }),enemies_.end());
}

std::shared_ptr<EnemyBase> EnemyManager::CreateInstance(const std::string& type)
{
	//敵の型に応じたクラスに実体を切り替える
	if (type == "Oni")
	{
		return std::make_shared<Oni>();
	}
	if (type == "BigMan")
	{
		return std::make_shared<BigMan>();
	}
	return nullptr;
}

