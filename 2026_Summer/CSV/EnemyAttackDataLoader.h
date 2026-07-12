#pragma once
#include "EnemyAttackData.h"
#include "EnemyAttackType.h"
#include <unordered_map>
#include <vector>
#include <string>

class EnemyAttackDataLoader
{
public:

	/// <summary>
	/// ロード
	/// </summary>
	/// <param name="filePath">ファイルのパス</param>
	/// <returns>ロードが成功したらtrue,失敗したらfalse</returns>
	bool Load(const std::wstring& filePath);

	/// <summary>
	/// 指定した種類の攻撃データを取得する
	/// </summary>
	/// <param name="type">攻撃の種類</param>
	/// <returns>攻撃のデータを返す</returns>
	const AttackData* GetAttackData(AttackType type) const;

	/// <summary>
	/// 敵の攻撃データを取得する
	/// </summary>
	/// <returns></returns>
	std::vector<const AttackData*> GetAllAttackData() const;

private:
	static std::wstring ToWString(const std::string& str);
	std::vector<std::string> SplitCsvLine(const std::string& line);

	static AttackType StringToAttackType(const std::string& str);
private:
	//攻撃ごとに攻撃の情報を保持するマップ
	std::unordered_map<AttackType, AttackData> attackDataMap_;
};

