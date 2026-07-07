#pragma once
#include "Math/Vector3.h"
#include <string>

//CSVに入っている敵のパラメータ
struct EnemyData
{
	//敵の種類
	std::string type_;

	//モデルのパス
	std::wstring modelPath_;

	//索敵範囲
	float searchRadius_ = 0.0f;

	//コライダーの半径
	float colliderRadius_ = 0.0f;

	//コライダーの高さ
	float colliderHeight_ = 0.0f;

	//各アニメーションの名前
	std::wstring chanceAnim_;
	std::wstring walkAnim_;
	std::wstring runAnim_;
	std::wstring attackAnim_;
	std::wstring damageAnim_;
	std::wstring deathAnim_;
};

