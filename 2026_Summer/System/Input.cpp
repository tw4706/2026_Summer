#include "Input.h"
#include<Dxlib.h>

Input::Input() :inputData_{}, lastInputData_{}, inputTable_{}
{
	inputTable_["up"] = { {PeripheralType::keyboard,KEY_INPUT_UP},
						{PeripheralType::pad1,PAD_INPUT_UP} };
	inputTable_["down"] = { {PeripheralType::keyboard,KEY_INPUT_DOWN},
						{PeripheralType::pad1,PAD_INPUT_DOWN} };
	inputTable_["left"] = { {PeripheralType::keyboard,KEY_INPUT_LEFT},
						{PeripheralType::pad1,PAD_INPUT_LEFT} };
	inputTable_["right"] = { {PeripheralType::keyboard,KEY_INPUT_RIGHT},
						{PeripheralType::pad1,PAD_INPUT_RIGHT} };
	inputTable_["cameraLeft"] = { {PeripheralType::keyboard,KEY_INPUT_X},
						{PeripheralType::pad1,PAD_INPUT_5} };
	inputTable_["cameraRight"] = { {PeripheralType::keyboard,KEY_INPUT_C},
						{PeripheralType::pad1,PAD_INPUT_6} };

	inputTable_["attack"] = { {PeripheralType::keyboard,KEY_INPUT_Z},
						{PeripheralType::pad1,PAD_INPUT_B} };
	inputTable_["retry"] = { {PeripheralType::keyboard,KEY_INPUT_RETURN},
						{PeripheralType::pad1,PAD_INPUT_A} };
	inputTable_["dodge"] = { {PeripheralType::keyboard,KEY_INPUT_D},
						{PeripheralType::pad1,PAD_INPUT_A} };
	inputTable_["fallPlayer"] = { {PeripheralType::keyboard,KEY_INPUT_F},
						{PeripheralType::pad1,PAD_INPUT_C} };

	//変な値が入らないように枠を開けておく
	for (const auto& input : inputTable_)
	{
		inputData_[input.first] = false;
		lastInputData_[input.first] = false;
	}
}

void Input::Update()
{
	//入力情報の取得
	char keyState[256];
	GetHitKeyStateAll(keyState);
	int padState = GetJoypadInputState(DX_INPUT_PAD1);
	lastInputData_ = inputData_;

	//アナログスティックの更新
	UpdateAnalogStick();

	//すべての入力イベントをチェック
	for (const auto& inputInfo : inputTable_)
	{
		auto& input = inputData_[inputInfo.first];
		for (const auto& state : inputInfo.second)
		{
			switch (state.type)
			{
			case PeripheralType::keyboard:
				input = keyState[state.id];
				break;
			case PeripheralType::pad1:
				input = (padState & state.id);
				break;
			}
			if (input)
			{
				//必須!
				break;
			}
		}
	}
}

bool Input::IsPressed(const char* name) const
{
	return inputData_.at(name);
}

bool Input::IsTriggered(const char* name) const
{
	return inputData_.at(name) && !lastInputData_.at(name);
}

void Input::UpdateAnalogStick()
{
	//左アナログスティック情報の取得
	int leftX, leftY;
	GetJoypadAnalogInput(&leftX, &leftY, DX_INPUT_PAD1);

	//スティックの上限・下限を設定する
	Vector3 stickLeft(leftX / 1000.0f, 0.0f, -leftY / 1000.0f);

	//デッドゾーン
	if (stickLeft.LengthSq() < 0.04f)
	{
		stickLeft = { 0,0,0 };
	}

	//正規化
	if (stickLeft.LengthSq() > 1.0f)
	{
		stickLeft = stickLeft.Normalize();
	}

	//線形補間(Lerp)
	stickLeft_.x_ = Vector3::Lerp(stickLeft_.x_, stickLeft.x_, 0.2f);
	stickLeft_.z_ = Vector3::Lerp(stickLeft_.z_, stickLeft.z_, 0.2f);

	//右アナログスティック情報の取得
	int rightX, rightY;
	GetJoypadAnalogInputRight(&rightX, &rightY, DX_INPUT_PAD1);

	//左の時と同様に上限・下限を設定する
	Vector3 targetRight(-rightX / 1000.0f, 0.0f, rightY / 1000.0f);

	//線形補間(Lerp)
	stickRight_.x_ = Vector3::Lerp(stickRight_.x_, targetRight.x_, 0.15f);
	stickRight_.z_ = Vector3::Lerp(stickRight_.z_, targetRight.z_, 0.15f);
}
