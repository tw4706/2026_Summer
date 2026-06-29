#pragma once
#include "Vector3.h"
#include <string>

//CSVに入っている敵のパラメータ
struct EnemyData
{
	//敵の種類
	std::string type_;

	//モデルのパス
	std::wstring modelPath_;

	//HP
	int hp_ = 0;

	//初期位置
	Vector3 pos_ = { 0.0f, 0.0f, 0.0f };

	//初期回転
	float rotateX_ = 0.0f;
	float rotateY_ = 0.0f;
	float rotateZ_ = 0.0f;

	//スケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	//索敵範囲
	float searchRadius_ = 500.0f;

	//コライダーの半径
	float colliderRadius_ = 70.0f;

	//コライダーの高さ
	float colliderHeight_ = 120.0f;

	//各アニメーションの名前
	std::wstring idleAnim_;
	std::wstring runAnim_;
	std::wstring attackAnim_;
	std::wstring damageAnim_;
	std::wstring deathAnim_;
};

