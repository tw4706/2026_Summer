#pragma once
#include<Dxlib.h>

class Vector3
{
public:
	float x_, y_, z_;

	Vector3();
	Vector3(float x, float y, float z);

	//変換するためのコンストラクタ
	Vector3(const VECTOR& v);

	virtual ~Vector3() {};

	//ベクトルの足し算
	Vector3 operator+(const Vector3& other) const;
	//ベクトルの引き算
	Vector3 operator-(const Vector3& other) const;
	//ベクトルの掛け算
	Vector3 operator*(float scalar) const;
	//ベクトルの割り算
	Vector3 operator/(float scalar) const;

	void operator+=(const Vector3& other);//加算
	void operator-=(const Vector3& other);//減算
	void operator*=(float scalar);//積
	void operator/=(float scalar);//商

	//ベクトルの正規化
	Vector3 Normalize() const;

	//ベクトルの長さを返す
	float Length() const;

	//ベクトルの長さの二乗を返す
	float LengthSq() const;

	//ベクトルの内積を返す
	float Dot(const Vector3& other) const;

	//ベクトルの外積を返す
	Vector3 Cross(const Vector3& other) const;

	//DxlibのVECTORに変換
	VECTOR ToDxlibVector()const { return VGet(x_, y_, z_); }

	//Lerp(線形補間)
	static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

	//float版
	static float Lerp(float a, float b, float t);
};

