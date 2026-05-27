#include "Vector3.h"
#include <cmath>

Vector3::Vector3() :
	x_(0.0f), y_(0.0f), z_(0.0f)
{
}

Vector3::Vector3(float x, float y, float z) :
	x_(x), y_(y), z_(z)
{
}

Vector3::Vector3(const VECTOR& v):
	x_(v.x),y_(v.y),z_(v.z)
{
}

//ベクトルの足し算
Vector3 Vector3::operator+(const Vector3& other) const
{
	return Vector3(x_ + other.x_, y_ + other.y_, z_ + other.z_);
}

//ベクトルの引き算
Vector3 Vector3::operator-(const Vector3& other) const
{
	return Vector3(x_ - other.x_, y_ - other.y_, z_ - other.z_);
}

//ベクトルの掛け算
Vector3 Vector3::operator*(float scalar) const
{
	return Vector3(x_ * scalar, y_ * scalar, z_ * scalar);
}

//ベクトルの割り算
Vector3 Vector3::operator/(float scalar) const
{
	return Vector3(x_ / scalar, y_ / scalar, z_ / scalar);
}

void Vector3::operator+=(const Vector3& other)
{
	x_ += other.x_;
	y_ += other.y_;
	z_ += other.z_;
}

void Vector3::operator-=(const Vector3& other)
{
	x_ -= other.x_;
	y_ -= other.y_;
	z_ -= other.z_;
}

void Vector3::operator*=(float scalar)
{
	x_ *= scalar;
	y_ *= scalar;
	z_ *= scalar;
}

void Vector3::operator/=(float scalar)
{
	x_ /= scalar;
	y_ /= scalar;
	z_ /= scalar;
}

//ベクトルの正規化
Vector3 Vector3::Normalize() const
{
	float len = Length();

	if (len < 0.0001f)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return Vector3(x_ / len, y_ / len, z_ / len);
}

//ベクトルの長さ
float Vector3::Length() const
{
	return std::sqrt(LengthSq());
}

//長さの二乗
float Vector3::LengthSq() const
{
	return x_ * x_ + y_ * y_ + z_ * z_;
}

//内積
float Vector3::Dot(const Vector3& other) const
{
	return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
}

//外積
Vector3 Vector3::Cross(const Vector3& other) const
{
	return Vector3(
		y_ * other.z_ - z_ * other.y_,
		z_ * other.x_ - x_ * other.z_,
		x_ * other.y_ - y_ * other.x_);
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t)
{
	return a + (b - a) * t;
}

float Vector3::Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}
