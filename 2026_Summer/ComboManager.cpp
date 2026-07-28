#include "ComboManager.h"
#include<string_view>

namespace
{
	//最初にコンボに入れておくコンボフレーム
	constexpr std::string_view kComboFirstKey = "Player|Combo";
}

ComboManager::ComboManager():
	comboCount_(0),
	maxCombo_(0),
	isNextComboRequested_(false)
{
}

bool ComboManager::Init(const std::wstring& filePath)
{
	bool result = loader_.Load(filePath);

	if (result)
	{
		maxCombo_ = loader_.GetComboCount();
	}
	else
	{
		maxCombo_ = 1;
	}

	comboCount_ = 1;
	isNextComboRequested_ = false;

	return result;
}

void ComboManager::OnAttackInput(float currentFrame)
{
	//入力の受付が可能になったら
	if (CanAcceptInput(currentFrame))
	{
		//リクエストをtrueにする
		isNextComboRequested_ = true;
	}
}

bool ComboManager::CanAcceptInput(float currentFrame) const
{
	const ComboData* data = GetCurrentComboData();
	if (!data) return false;

	return currentFrame >= static_cast<float>(data->inputAcceptStartFrame) &&
		currentFrame <= static_cast<float>(data->inputAcceptEndFrame);
}

bool ComboManager::CanTransToNextCombo(float currentFrame) const
{
	const ComboData* data = GetCurrentComboData();
	if (!data) return false;

	return currentFrame >= static_cast<float>(data->comboTransitionFrame);
}

void ComboManager::AdvancedCombo()
{
	//最大コンボが今のコンボカウントより少ない場合
	if (comboCount_ < maxCombo_)
	{
		//カウントする
		comboCount_++;
	}

	//リクエストをfalse
	isNextComboRequested_ = false;
}

void ComboManager::ResetCombo()
{
	comboCount_ = 1;
	isNextComboRequested_ = false;
}

const ComboData* ComboManager::GetCurrentComboData() const
{
	std::string key = MakeComboKey(comboCount_);
	return loader_.GetComboData(key);
}

bool ComboManager::IsMaxCombo() const
{
	return comboCount_ >= maxCombo_;
}

std::string ComboManager::MakeComboKey(int comboCount) const
{
	return std::string(kComboFirstKey) + std::to_string(comboCount);
}
