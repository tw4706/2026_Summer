#include "AttackSelect.h"

const AttackData* AttackSelect::ChooseWeighted(const std::vector<const AttackData*>& datas)
{
	if (datas.empty())
	{
		return nullptr;
	}

	float totalWeight = 0.0f;
	for (const auto* data : datas)
	{
		totalWeight += data->weight_;
	}

	if (totalWeight <= 0.0f)
	{
		return datas[0];
	}

	//—”¶¬(‚±‚Ì—”¶¬‚ÅUŒ‚‚Ìƒpƒ^[ƒ“‚ðŒˆ‚ß‚é)
	float r = static_cast<float>(rand()) / RAND_MAX * totalWeight;

	float accumulated = 0.0f;
	for (const auto* data : datas)
	{
		accumulated += data->weight_;
		if (r <= accumulated)
		{
			return data;
		}
	}

	return datas.back();
}
