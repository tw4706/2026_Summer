#include "EnemySpawnDataLoader.h"
#include <fstream>
#include <sstream>

bool EnemySpawnDataLoader::Load(const std::wstring& filePath)
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

		EnemySpawnData data;
		int i = 0;

		//エリアID
		data.areaId_ = std::stoi(cols[i++]);

		//敵の種類
		data.type_ = cols[i++];

		//HP
		data.hp_ = std::stoi(cols[i++]);

		//生成座標
		data.spawnPos_.x_ = std::stof(cols[i++]);
		data.spawnPos_.y_ = std::stof(cols[i++]);
		data.spawnPos_.z_ = std::stof(cols[i++]);

		//回転角度
		data.rotateX = std::stof(cols[i++]);
		data.rotateY = std::stof(cols[i++]);
		data.rotateZ = std::stof(cols[i++]);

		//拡大率
		data.scale_.x_ = std::stof(cols[i++]);
		data.scale_.y_ = std::stof(cols[i++]);
		data.scale_.z_ = std::stof(cols[i++]);

		spawnDataMap_[data.areaId_].push_back(data);
	}

	return true;
}

std::vector<const EnemySpawnData*> EnemySpawnDataLoader::GetSpawnDataByArea(int areaId) const
{
	std::vector<const EnemySpawnData*> result;

	auto it = spawnDataMap_.find(areaId);
	if (it == spawnDataMap_.end())
	{
		return result; //空のvectorを返す
	}

	for (const auto& data : it->second)
	{
		result.push_back(&data);
	}

	return result;
}

std::wstring EnemySpawnDataLoader::ToWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::vector<std::string> EnemySpawnDataLoader::SplitCsvLine(const std::string& line)
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