#pragma once

class PlayerActionCounter
{
public:
	/// <summary>
	/// ガード行動のカウンタ関数
	/// </summary>
	void RecordGuard();

	/// <summary>
	/// 回避行動のカウンタ関数
	/// </summary>
	void RecordDodge();

	/// <summary>
	/// 攻撃行動のカウンタ関数
	/// </summary>
	void RecordAttack();

	float GetGuardRatio()  const { return totalCount_ > 0 ? static_cast<float>(guardCount_) / totalCount_ : 0.0f; }
	float GetDodgeRatio()  const { return totalCount_ > 0 ? static_cast<float>(dodgeCount_) / totalCount_ : 0.0f; }
	float GetAttackRatio() const { return totalCount_ > 0 ? static_cast<float>(attackCount_) / totalCount_ : 0.0f; }

private:
	void DecayIfNeeded();

	int guardCount_ = 0;	//ガードカウンタ
	int dodgeCount_ = 0;	//回避カウンタ
	int attackCount_ = 0;	//攻撃カウンタ
	int totalCount_ = 0;	//トータルのカウント
};

