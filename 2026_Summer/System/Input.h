#pragma once
#include"Math/Vector3.h"
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
	pad1,
	padXInput,
};

struct InputState
{
	PeripheralType type;	//入力された機器の種別
	int id;					//入力情報が入る(キーボードの場合はインデックス、パッドの場合はビット)
};

class Input
{
public:

	/// <summary>
	/// インスタンスの作成
	/// </summary>
	/// <returns>インスタンス</returns>
	static Input& GetInstance();

private:
	//初期化処理
	Input();

	//コピー禁止
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

public:
	//更新処理
	void Update();
	//ボタンが押されたか
	bool IsPressed(const char* name)const;

	//ボタンが今押されたかどうか
	bool IsTriggered(const char* name)const;

	//ボタンが離されたかどうか
	bool IsReleased(const char* name)const;

	//アナログスティックの更新
	void UpdateAnalogStick();

	//移動の入力が現在あるかどうか
	bool HasMoveInput() const;

	//カメラの向きを考慮しない入力の向きと強さのベクトルを返す
	Vector3 GetRawMoveInput() const;

	//スティック情報の取得
	Vector3 GetStickLeft()const { return stickLeft_; }
	Vector3 GetStickRight()const { return stickRight_; }

	//スティックがはじかれているかの取得
	bool IsRightStickFlickLeft() const { return isRightStickFlickL_; }
	bool IsRightStickFlickRight() const { return isRightStickFlickR_; }
private:
	std::map<std::string, std::vector<InputState>>inputTable_;	//イベント名と実際の入力の対応表
	std::map<std::string, bool>inputData_;						//実際に入力されたかどうかのデータ
	std::map<std::string, bool>lastInputData_;					//最後に入力されたかどうかのデータ

	Vector3 stickLeft_ = { 0.0f,0.0f,0.0f };
	Vector3 stickRight_ = { 0.0f,0.0f,0.0f };

	//XInputの取得
	XINPUT_STATE xInputState_={};
	//パッドが接続できているかどうか
	bool isXInputConnected_ =false;

	//前フレームの右スティックの記憶用変数
	bool lastRightStickFlickL_ = false;
	bool lastRightStickFlickR_ = false;

	//今のフレームでスティックが弾かれたかどうかのフラグ
	bool isRightStickFlickL_ = false;
	bool isRightStickFlickR_ = false;
};

