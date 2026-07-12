#include "EnemyAttackDataLoader.h"
#include <fstream>
#include <sstream>

bool EnemyAttackDataLoader::Load(const std::wstring& filePath)
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
		if (cols.size() < 7)continue;

		AttackData data;
		int i = 0;

		//攻撃ID
		data.id_ = std::stoi(cols[i++]);

		//攻撃の種類
		data.type_ = StringToAttackType(cols[i++]);

		//予備動作から攻撃に移行するフレーム
		data.attackTransFrame_ = std::stof(cols[i++]);

		//コライダーの半径
		data.colliderRadius_ = std::stof(cols[i++]);

		//コライダーの高さ
		data.colliderHeight_ = std::stof(cols[i++]);

		//ダメージ量
		data.damage_ = std::stof(cols[i++]);

		//選択される重み
		data.weight_ = std::stof(cols[i++]);

		attackDataMap_[data.type_] = data;
	}

	return true;
}

const AttackData* EnemyAttackDataLoader::GetAttackData(AttackType type) const
{
	auto it = attackDataMap_.find(type);
	if (it == attackDataMap_.end())
	{
		return nullptr;
	}

	return &it->second;
}

std::vector<const AttackData*> EnemyAttackDataLoader::GetAllAttackData() const
{
	std::vector<const AttackData*> result;

	for (const auto& pair : attackDataMap_)
	{
		result.push_back(&pair.second);
	}

	return result;
}

std::wstring EnemyAttackDataLoader::ToWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::vector<std::string> EnemyAttackDataLoader::SplitCsvLine(const std::string& line)
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

AttackType EnemyAttackDataLoader::StringToAttackType(const std::string& str)
{
	if (str == "NormalAttack") return AttackType::NormalAttack;
	if (str == "JumpAttack")   return AttackType::JumpAttack;

	//当てはまらない場合は通常攻撃を返す
	return AttackType::NormalAttack;
}