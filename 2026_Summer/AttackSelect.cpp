#include "AttackSelect.h"

const AttackData* AttackSelect::ChooseWeighted(const std::vector<const AttackData*>& datas, const PlayerActionCounter& counter)
{
	//攻撃の候補がない場合は何もしない
	if (datas.empty()) return nullptr;

	//プレイヤーがガードしているカウントを記録しているカウンタを取得
	float guardRatio = counter.GetGuardRatio();

	std::vector<float> weights;
	weights.reserve(datas.size());
	float totalWeight = 0.0f;

	//各攻撃の重み(優先度・比率)を計算
	for (const auto* data : datas)
	{
		float w = data->weight_;

		//プレイヤーがガードをするほどダメージの高いジャンプ攻撃を選ばれやすくする
		if (data->type_ == AttackType::JumpAttack)
		{
			w *= 1.0f + guardRatio * 2.0f;//ずっとガードしているなら確率が2倍
		}

		//配列に入れて合計の重みを代入
		weights.push_back(w);
		totalWeight += w;
	}

	//合計の重みが0の場合は要素の最初を返す
	if (totalWeight <= 0.0f) return datas[0];

	//確率の計算(乱数)
	//weightが大きい攻撃ほど当たる確率が多くなる
	float r = static_cast<float>(rand()) / RAND_MAX * totalWeight;
	float weightRangeEnd = 0.0f;
	for (size_t i = 0; i < datas.size(); ++i)
	{
		weightRangeEnd += weights[i];
		if (r <= weightRangeEnd) return datas[i];
	}
	return datas.back();
}
