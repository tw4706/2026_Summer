#include "ComboDataLoader.h"
#include <fstream>
#include <sstream>
#include <vector>

bool ComboDataLoader::Load(const std::wstring& filePath)
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
		if (cols.size() < 9)continue;

		ComboData data;
		int i = 0;

		//アニメーション名
		const std::string animNameKey = cols[i++];
		data.animName= ToWString(animNameKey);

		//エフェクトの開始・終了フレーム
		data.effectStartFrame= std::stoi(cols[i++]);
		data.effectEndFrame= std::stoi(cols[i++]);

		//攻撃コライダーの開始・終了フレーム
		data.colliderStartFrame = std::stoi(cols[i++]);
		data.colliderEndFrame = std::stoi(cols[i++]);

		//次のコンボへの入力受付開始・終了フレーム
		data.inputAcceptStartFrame = std::stoi(cols[i++]);
		data.inputAcceptEndFrame = std::stoi(cols[i++]);

		//次のコンボ遷移可能になるフレーム
		data.comboTransitionFrame= std::stoi(cols[i++]);

		//コンボ再生時のSE再生フレーム
		data.comboPlaySEFrame = std::stoi(cols[i++]);

		comboDataMap_[animNameKey] = data;
	};
	return true;
}

const ComboData* ComboDataLoader::GetComboData(const std::string& key) const
{
	auto it = comboDataMap_.find(key);
	if (it == comboDataMap_.end())
	{
		return nullptr;
	}

	return &it->second;
}

int ComboDataLoader::GetComboCount() const
{
	return static_cast<int>(comboDataMap_.size());
}

std::wstring ComboDataLoader::ToWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::vector<std::string> ComboDataLoader::SplitCsvLine(const std::string& line)
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
