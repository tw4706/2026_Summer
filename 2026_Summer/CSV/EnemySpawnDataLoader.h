#pragma once
#include "EnemySpawnData.h"
#include <unordered_map>
#include <vector>
#include <string>

class EnemySpawnDataLoader
{
public:

	/// <summary>
	/// ロード
	/// </summary>
	/// <param name="filePath">ファイルのパス</param>
	/// <returns>ロードが成功したらtrue,失敗したらfalse</returns>
	bool Load(const std::wstring& filePath);

	/// <summary>
	/// 指定したエリアIDの配置座標を取得する
	/// </summary>
	/// <param name="areaId">エリアID</param>
	/// <returns></returns>
	std::vector<const EnemySpawnData*> GetSpawnDataByArea(int areaId) const;

private:
	static std::wstring ToWString(const std::string& str);
	std::vector<std::string> SplitCsvLine(const std::string& line);

private:
	//エリアIdごとに複数の配置データを保持するマップ
	std::unordered_map<int, std::vector<EnemySpawnData>> spawnDataMap_;
};

