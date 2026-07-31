#include "Input.h"
#include<Dxlib.h>

namespace
{
	//はじきの閾値
	constexpr float kFlickThreshold = 0.6f;

	//左右スティックのlerpの割合
	constexpr float kStickLeftLerpRate = 0.2f;
	constexpr float kStickRightLerpRate = 0.15f;

	//左スティックの閾値
	constexpr float kStickLeftThereshould = 0.09f;

	//左スティックのデッドゾーン
	constexpr float kSthickLeftDeadZone = 0.04f;

	//右スティックのデッドゾーン
	constexpr float kSthickRightDeadZone = 0.04f;
}

Input& Input::GetInstance()
{
	static Input instance;
	return instance;
}

Input::Input() :inputData_{}, lastInputData_{}, inputTable_{}
{
	inputTable_["up"] = { {PeripheralType::keyboard,KEY_INPUT_UP},
						{PeripheralType::pad1,PAD_INPUT_UP},
						{PeripheralType::padXInput,XINPUT_BUTTON_DPAD_DOWN} };
	inputTable_["down"] = { {PeripheralType::keyboard,KEY_INPUT_DOWN},
						{PeripheralType::pad1,PAD_INPUT_DOWN},
						{PeripheralType::padXInput,XINPUT_BUTTON_DPAD_UP} };
	inputTable_["left"] = { {PeripheralType::keyboard,KEY_INPUT_LEFT},
						{PeripheralType::pad1,PAD_INPUT_LEFT},
						{PeripheralType::padXInput,XINPUT_BUTTON_DPAD_LEFT} };
	inputTable_["right"] = { {PeripheralType::keyboard,KEY_INPUT_RIGHT},
						{PeripheralType::pad1,PAD_INPUT_RIGHT},
						{PeripheralType::padXInput,XINPUT_BUTTON_DPAD_RIGHT} };

	inputTable_["lockOn"] = { {PeripheralType::keyboard,KEY_INPUT_L},
						{PeripheralType::pad1,PAD_INPUT_Y},
						{PeripheralType::padXInput,XINPUT_BUTTON_RIGHT_THUMB} };
	inputTable_["attack"] = { {PeripheralType::keyboard,KEY_INPUT_Z},
						{PeripheralType::pad1,PAD_INPUT_4},
						{PeripheralType::padXInput,XINPUT_BUTTON_Y} };
	inputTable_["guard"] = { {PeripheralType::keyboard,KEY_INPUT_G},
						//{PeripheralType::pad1,PAD_INPUT_6},
						{PeripheralType::padXInput,XINPUT_BUTTON_RIGHT_SHOULDER} };
	inputTable_["dodge"] = { {PeripheralType::keyboard,KEY_INPUT_D},
						{PeripheralType::pad1,PAD_INPUT_7},
						{PeripheralType::padXInput,XINPUT_BUTTON_X} };
	inputTable_["jump"] = { {PeripheralType::keyboard,KEY_INPUT_SPACE},
						{PeripheralType::pad1,PAD_INPUT_A},
						{PeripheralType::padXInput,XINPUT_BUTTON_A} };
	inputTable_["next"] = { {PeripheralType::keyboard,KEY_INPUT_RETURN},
						{PeripheralType::pad1,PAD_INPUT_A},
						{PeripheralType::padXInput,XINPUT_BUTTON_A} };
	inputTable_["debugWarp"] = { {PeripheralType::keyboard,KEY_INPUT_D} };

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
	int padState = GetJoypadInputState(DX_INPUT_PAD1);
	GetHitKeyStateAll(keyState);
	lastInputData_ = inputData_;

	//パッドが接続できているかどうか
	isXInputConnected_ = (GetJoypadXInputState(DX_INPUT_PAD1, &xInputState_) == 0);

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
			case PeripheralType::padXInput:
				if (isXInputConnected_)
				{
					input = (xInputState_.Buttons[state.id] != 0);
				}
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

bool Input::IsReleased(const char* name) const
{
	return !inputData_.at(name) && lastInputData_.at(name);
}

void Input::UpdateAnalogStick()
{
	Vector3 stickLeft;
	Vector3 targetRight;

	//XInput対応コントローラが接続されているとき
	if (isXInputConnected_)
	{
		//XInputの生値(-32768～32767)を正規化して使う
		stickLeft = Vector3(xInputState_.ThumbLX / 32768.0f, 0.0f, xInputState_.ThumbLY / 32768.0f);
		targetRight = Vector3(-xInputState_.ThumbRX / 32768.0f, 0.0f, xInputState_.ThumbRY / 32768.0f);
	}
	else//XInput対応コントローラ以外の場合
	{
		//左スティックの取得
		int leftX, leftY;
		GetJoypadAnalogInput(&leftX, &leftY, DX_INPUT_PAD1);
		stickLeft= Vector3(leftX / 1000.0f, 0.0f, -leftY / 1000.0f);

		//右スティックの取得
		int rightX, rightY;
		GetJoypadAnalogInputRight(&rightX, &rightY, DX_INPUT_PAD1);
		targetRight=Vector3(-rightX / 1000.0f, 0.0f, rightY / 1000.0f);
	}

	//左スティックのデッドゾーン
	if (stickLeft.LengthSq() < kSthickLeftDeadZone)
	{
		stickLeft = { 0,0,0 };
	}
	//右スティックのデッドゾーン
	if (targetRight.LengthSq() < kSthickRightDeadZone)
	{
		targetRight = { 0,0,0 };
	}

	//正規化
	if (stickLeft.LengthSq() > 1.0f)
	{
		stickLeft = stickLeft.Normalize();
	}

	//線形補間(Lerp)
	stickLeft_.x_ = Vector3::Lerp(stickLeft_.x_, stickLeft.x_, kStickLeftLerpRate);
	stickLeft_.z_ = Vector3::Lerp(stickLeft_.z_, stickLeft.z_, kStickLeftLerpRate);

	//線形補間(Lerp)
	stickRight_.x_ = Vector3::Lerp(stickRight_.x_, targetRight.x_, kStickRightLerpRate);
	stickRight_.z_ = Vector3::Lerp(stickRight_.z_, targetRight.z_, kStickRightLerpRate);

	float currentX = stickRight_.x_;

	//左方向へのフリック
	//閾値より小さいときは左
	bool currentLeft = (currentX < -kFlickThreshold);
	isRightStickFlickL_ = (currentLeft && !lastRightStickFlickL_);
	lastRightStickFlickL_ = currentLeft;

	//右方向へのフリック
	//閾値より大きいときは右
	bool currentRight = (currentX > kFlickThreshold);
	isRightStickFlickR_ = (currentRight && !lastRightStickFlickR_);
	lastRightStickFlickR_ = currentRight;
}

bool Input::HasMoveInput() const
{
	//方向キーのチェック
	if (IsPressed("up") || IsPressed("down") || IsPressed("left") || IsPressed("right"))
	{
		return true;
	}

	//左アナログスティックのチェック
	//スティック推している長さが基準以上なら入力があるとしている
	if (stickLeft_.LengthSq() > kStickLeftThereshould)
	{
		return true;
	}

	return false;
}

Vector3 Input::GetRawMoveInput() const
{
	//左アナログスティックの入力
	Vector3 inputDir = stickLeft_;

	//アナログスティックが入力されていないなら
	if (inputDir.LengthSq() <= 0.0f)
	{
		//速度を適用する
		if (IsPressed("up"))    inputDir.z_ += 1.0f;
		if (IsPressed("down"))  inputDir.z_ -= 1.0f;
		if (IsPressed("left"))  inputDir.x_ -= 1.0f;
		if (IsPressed("right")) inputDir.x_ += 1.0f;

		//正規化
		if (inputDir.LengthSq() > 1.0f)
		{
			inputDir = inputDir.Normalize();
		}
	}

	return inputDir;
}
