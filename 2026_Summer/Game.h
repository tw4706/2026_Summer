#pragma once
#include<Dxlib.h>

// ゲーム全体で使用する定数
namespace Game
{
	// 画面情報
	constexpr int kScreenWidth = 1280;
	constexpr int kScreenHeight = 720;
	constexpr int kColorBitNum = 32;

	//半分が欲しい場合があるので
	constexpr float kHalf = 0.5f;

	//各色の定数
	const unsigned int kRedColor = GetColor(255, 0, 0);
	const unsigned int kOrangeColor = GetColor(255, 128, 0);
	const unsigned int kBlueColor = GetColor(0, 0, 255);
	const unsigned int kLightBlueColor = GetColor(0, 255, 255);
	const unsigned int kGreenColor = GetColor(0, 255, 0);
	const unsigned int kYellowColor = GetColor(255, 255, 0);
	const unsigned int kBlackColor = GetColor(0, 0, 0);
	const unsigned int kWhiteColor = GetColor(255, 255, 255);

	inline int kFontUIHandle = -1;
}
