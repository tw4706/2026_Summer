#pragma once
#include "EnemyData.h"
#include <unordered_map>
#include <string>

class EnemyDataLoader
{
public:
	/// <summary>
	/// CSVファイルを読み込み、type(種類)名をキーにしたマップを構築する
	/// </summary>
	/// <param name="filePath">CSVのファイルのパス</param>
	/// <returns>読み込みに成功したかどうかを返す</returns>
	bool Load(const std::string& filePath);

	/// <summary>
	/// 敵の種類から敵データを取得する
	/// </summary>
	const EnemyData* GetEnemyData(const std::string& type) const;

private:

	/// <summary>
	/// string型の文字列をwstring型に変換する関数
	/// </summary>
	/// <param name="str">変換するstring型の文字列</param>
	/// <returns>変換後のwstring型の文字列</returns>
	static std::wstring ToWString(const std::string& str);

	/// <summary>
	/// カンマ区切りで1行ずつ分割する関数
	/// </summary>
	/// <param name="line">CSVの行</param>
	/// <returns></returns>
	std::vector<std::string> SplitCsvLine(const std::string& line);

private:
	//敵パラメータを格納するマップ
	std::unordered_map<std::string, EnemyData> enemyDataMap_;
};

