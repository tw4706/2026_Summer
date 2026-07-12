#include "EnemyStatus.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "EnemySpawnDataLoader.h"

bool EnemyStatus::Load(const std::wstring& filePath)
{
	std::ifstream ifs(filePath);
	if (!ifs.is_open())
	{
		return false;
	}

	std::string line;
	//1行目はヘッダなので飛ばす
	std::getline(ifs, line);

	while (std::getline(ifs, line))
	{
		if (line.empty())continue;

		auto cols = SplitCsvLine(line);
		//列が足りない行はスキップ
		if (cols.size() < 12)continue;

		EnemyData data;
		int i = 0;

		//敵の種類
		data.type_ = cols[i++];

		//モデルのパス
		data.modelPath_ = ToWString(cols[i++]);

		//索敵範囲
		data.searchRadius_ = std::stof(cols[i++]);

		//コライダーの半径と高さ
		data.colliderRadius_ = std::stof(cols[i++]);
		data.colliderHeight_ = std::stof(cols[i++]);

		//アニメーション名
		data.chanceAnim_ = ToWString(cols[i++]);
		data.walkAnim_ = ToWString(cols[i++]);
		data.runAnim_ = ToWString(cols[i++]);
		data.attackAnim_ = ToWString(cols[i++]);
		data.jumpAttackAnim_ = ToWString(cols[i++]);
		data.damageAnim_ = ToWString(cols[i++]);
		data.deathAnim_ = ToWString(cols[i++]);

		enemyDataMap_[data.type_] = data;
	}

	return true;
}

const EnemyData* EnemyStatus::GetEnemyData(const std::string& type) const
{
	auto it = enemyDataMap_.find(type);
	if (it == enemyDataMap_.end())
	{
		return nullptr;
	}
	return &it->second;
}

std::wstring EnemyStatus::ToWString(const std::string& str)
{
	return std::wstring(str.begin(),str.end());
}

std::vector<std::string> EnemyStatus::SplitCsvLine(const std::string& line)
{
	std::vector<std::string> result;
	std::stringstream ss(line);
	std::string cell;
	while (std::getline(ss, cell, ','))
	{
		result.push_back(cell);
	}
	return result;
}
