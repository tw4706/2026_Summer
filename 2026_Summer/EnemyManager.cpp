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

std::shared_ptr<EnemyBase> EnemyManager::SpawnEnemy(const std::string& pathType)
{
	const EnemyData* pData = dataLoader_.GetEnemyData(pathType);
	if (!pData)
	{
		return nullptr;
	}

	auto enemy = CreateInstance(pathType);
	if (!enemy)
	{
		return nullptr;
	}

	//CSVで入力したパラメータを適用してから初期化を行う
	enemy->ApplyData(*pData);
	enemy->Init();

	enemies_.push_back(enemy);
	return enemy;
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

void EnemyManager::RemoveEnemy(const std::shared_ptr<EnemyBase>& enemy)
{
	//削除処理
	enemies_.erase(
		std::remove(enemies_.begin(), enemies_.end(), enemy),
		enemies_.end());
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

