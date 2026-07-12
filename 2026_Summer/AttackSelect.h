#pragma once
#include "CSV/EnemyAttackData.h"
#include "PlayerActionCounter.h"
#include <vector>

class AttackSelect
{
public:
	/// <summary>
	/// weight_を使った重み付き抽選で攻撃データを選ぶ
	/// </summary>
	/// <param name="data">選択肢として選ばれる攻撃データ</param>
	/// <returns>選ばれた攻撃データのポインタ、候補が無ければnullptr</returns>
	static const AttackData* ChooseWeighted(const std::vector<const AttackData*>& datas	,const PlayerActionCounter& counter);
};

