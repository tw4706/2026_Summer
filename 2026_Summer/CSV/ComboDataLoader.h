#pragma once
#include"ComboData.h"
#include<string>
#include<unordered_map>

class ComboDataLoader
{
public:
	/// <summary>
	/// ロード
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>ロード出来ていたらtrue,出来ていなかったらfalse</returns>
	bool Load(const std::wstring& filePath);

	/// <summary>
	/// コンボデータを取得する
	/// </summary>
	/// <param name="comboData">コンボデータの文字列</param>
	/// <returns>コンボの文字列を返す</returns>
	const ComboData* GetComboData(const std::string& key)const;
	
	/// <summary>
	/// コンボ数のカウント
	/// </summary>
	int  GetComboCount()const;

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
	//コンボのデータを格納しておくためのマップ
	std::unordered_map<std::string, ComboData>comboDataMap_;
};

