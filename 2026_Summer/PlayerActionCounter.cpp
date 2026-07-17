#include "PlayerActionCounter.h"

namespace
{
	//確率を一定に保つためにカウントをリセットする基準
	constexpr int kDecayThreshold = 20;
}

void PlayerActionCounter::RecordGuard()
{
	guardCount_++;
	totalCount_++;
}

void PlayerActionCounter::RecordDodge()
{
	dodgeCount_++;
	totalCount_++;
}

void PlayerActionCounter::RecordAttack()
{
	attackCount_++;
	totalCount_++;
}

//一定数を超えたら半分にして確立を偏らせないようにする
void PlayerActionCounter::DecayIfNeeded()
{
	if (totalCount_ > kDecayThreshold)
	{
		guardCount_ /= 2;
		dodgeCount_ /= 2;
		attackCount_ /= 2;
		totalCount_ /= 2;
	}
}
