#pragma once
#include"Vector3.h"
#include<array>
#include<map>
#include<string>
#include<vector>

/// <summary>
/// 周辺機器種別
/// </summary>
enum class PeripheralType
{
	keyboard,
	pad1
};

struct InputState
{
	PeripheralType type;	//入力された機器の種別
	int id;					//入力情報が入る(キーボードの場合はインデックス、パッドの場合はビット)
};

class Input
{
public:
	//初期化処理
	Input();
	//更新処理
	void Update();
	//ボタンが押されたか
	bool IsPressed(const char* name)const;
	//ボタンが今押されたかどうか
	bool IsTriggered(const char* name)const;

	//アナログスティックの更新
	void UpdateAnalogStick();

	//スティック情報の取得
	Vector3 GetStickLeft()const { return stickLeft_; }
	Vector3 GetStickRight()const { return stickRight_; }
private:
	std::map<std::string, std::vector<InputState>>inputTable_;	//イベント名と実際の入力の対応表
	std::map<std::string, bool>inputData_;						//実際に入力されたかどうかのデータ
	std::map<std::string, bool>lastInputData_;					//最後に入力されたかどうかのデータ

	Vector3 stickLeft_ = { 0.0f,0.0f,0.0f };
	Vector3 stickRight_ = { 0.0f,0.0f,0.0f };
};

