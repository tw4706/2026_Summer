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

bool EnemyManager::LoadWayPointData(const std::wstring& path)
{
	return wayPointLoader_.Load(path);
}

std::shared_ptr<EnemyBase> EnemyManager::SpawnEnemy(const std::string& pathType)
{
	//CSVのType列に応じたデータの取得
	const EnemyData* pData = dataLoader_.GetEnemyData(pathType);

	//データが取得できない場合はnullptrを返す
	if (!pData)
	{
		return nullptr;
	}

	//敵の方に応じるインスタンスの作成
	auto enemy = CreateInstance(pathType);

	//敵のインスタンスが作成できない場合はnullptrを返す
	if (!enemy)
	{
		return nullptr;
	}

	//CSVで入力したパラメータを適用
	enemy->ApplyData(*pData);

	//WayPointのセット
	enemy->SetWayPointLoader(&wayPointLoader_);

	//敵の初期化
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

