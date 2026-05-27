#include "Matrix4x4.h"
#include <cmath>

Matrix4x4::Matrix4x4():
	x0_(0.0f), x1_(0.0f), x2_(0.0f), x3_(0.0f),
	y0_(0.0f), y1_(0.0f), y2_(0.0f), y3_(0.0f),
	z0_(0.0f), z1_(0.0f), z2_(0.0f), z3_(0.0f),
	w0_(0.0f), w1_(0.0f), w2_(0.0f), w3_(0.0f)
{
}

Matrix4x4::Matrix4x4(float x0, float x1, float x2, float x3, 
	float y0, float y1, float y2, float y3, 
	float z0, float z1, float z2, float z3,
	float w0, float w1, float w2, float w3):
	x0_(x0), x1_(x1), x2_(x2), x3_(x3),
	y0_(y0), y1_(y1), y2_(y2), y3_(y3),
	z0_(z0), z1_(z1), z2_(z2), z3_(z3),
	w0_(w0), w1_(w1), w2_(w2), w3_(w3)
{
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const
{
	Matrix4x4 mat;

	//1行目
	mat.x0_ = x0_ + other.x0_;
	mat.x1_ = x1_ + other.x1_;
	mat.x2_ = x2_ + other.x2_;
	mat.x3_ = x3_ + other.x3_;

	//2行目
	mat.y0_ = y0_ + other.y0_;
	mat.y1_ = y1_ + other.y1_;
	mat.y2_ = y2_ + other.y2_;
	mat.y3_ = y3_ + other.y3_;

	//3行目
	mat.z0_ = z0_ + other.z0_;
	mat.z1_ = z1_ + other.z1_;
	mat.z2_ = z2_ + other.z2_;
	mat.z3_ = z3_ + other.z3_;

	//4行目
	mat.w0_ = w0_ + other.w0_;
	mat.w1_ = w1_ + other.w1_;
	mat.w2_ = w2_ + other.w2_;
	mat.w3_ = w3_ + other.w3_;

	return mat;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const
{
	Matrix4x4 mat;

	//1行目
	mat.x0_ = x0_ - other.x0_;
	mat.x1_ = x1_ - other.x1_;
	mat.x2_ = x2_ - other.x2_;
	mat.x3_ = x3_ - other.x3_;

	//2行目
	mat.y0_ = y0_ - other.y0_;
	mat.y1_ = y1_ - other.y1_;
	mat.y2_ = y2_ - other.y2_;
	mat.y3_ = y3_ - other.y3_;

	//3行目
	mat.z0_ = z0_ - other.z0_;
	mat.z1_ = z1_ - other.z1_;
	mat.z2_ = z2_ - other.z2_;
	mat.z3_ = z3_ - other.z3_;

	//4行目
	mat.w0_ = w0_ - other.w0_;
	mat.w1_ = w1_ - other.w1_;
	mat.w2_ = w2_ - other.w2_;
	mat.w3_ = w3_ - other.w3_;

	return mat;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
	Matrix4x4 mat;

	//1行目
	mat.x0_ = x0_ * other.x0_ + x1_ * other.y0_ + x2_ * other.z0_ + x3_ * other.w0_;
	mat.x1_ = x0_ * other.x1_ + x1_ * other.y1_ + x2_ * other.z1_ + x3_ * other.w1_;
	mat.x2_ = x0_ * other.x2_ + x1_ * other.y2_ + x2_ * other.z2_ + x3_ * other.w2_;
	mat.x3_ = x0_ * other.x3_ + x1_ * other.y3_ + x2_ * other.z3_ + x3_ * other.w3_;

	//2行目	
	mat.y0_ = y0_ * other.x0_ + y1_ * other.y0_ + y2_ * other.z0_ + y3_ * other.w0_;
	mat.y1_ = y0_ * other.x1_ + y1_ * other.y1_ + y2_ * other.z1_ + y3_ * other.w1_;
	mat.y2_ = y0_ * other.x2_ + y1_ * other.y2_ + y2_ * other.z2_ + y3_ * other.w2_;
	mat.y3_ = y0_ * other.x3_ + y1_ * other.y3_ + y2_ * other.z3_ + y3_ * other.w3_;

	//3行目
	mat.z0_ = z0_ * other.x0_ + z1_ * other.y0_ + z2_ * other.z0_ + z3_ * other.w0_;
	mat.z1_ = z0_ * other.x1_ + z1_ * other.y1_ + z2_ * other.z1_ + z3_ * other.w1_;
	mat.z2_ = z0_ * other.x2_ + z1_ * other.y2_ + z2_ * other.z2_ + z3_ * other.w2_;
	mat.z3_ = z0_ * other.x3_ + z1_ * other.y3_ + z2_ * other.z3_ + z3_ * other.w3_;

	//4行目
	mat.w0_ = w0_ * other.x0_ + w1_ * other.y0_ + w2_ * other.z0_ + w3_ * other.w0_;
	mat.w1_ = w0_ * other.x1_ + w1_ * other.y1_ + w2_ * other.z1_ + w3_ * other.w1_;
	mat.w2_ = w0_ * other.x2_ + w1_ * other.y2_ + w2_ * other.z2_ + w3_ * other.w2_;
	mat.w3_ = w0_ * other.x3_ + w1_ * other.y3_ + w2_ * other.z3_ + w3_ * other.w3_;

	return mat;
}

Matrix4x4 Matrix4x4::RotateX(float angle)
{
	Matrix4x4 mat;

	mat.x0_ = 1.0f;
	mat.y1_ = cosf(angle);
	mat.y2_ = -sinf(angle);
	mat.z1_ = sinf(angle);
	mat.z2_ = cosf(angle);
	mat.w3_ = 1.0f;

	return mat;
}

Matrix4x4 Matrix4x4::RotateY(float angle)
{
	Matrix4x4 mat;

	mat.x0_ = cosf(angle);
	mat.x2_ = sinf(angle);
	mat.y1_ = 1.0f;
	mat.z0_ = -sinf(angle);
	mat.z2_ = cosf(angle);
	mat.w3_ = 1.0f;

	return mat;
}

Matrix4x4 Matrix4x4::RotateZ(float angle)
{
	Matrix4x4 mat;

	mat.x0_ = cosf(angle);
	mat.x1_ = -sinf(angle);
	mat.y0_ = sinf(angle);
	mat.y1_ = cosf(angle);
	mat.z2_ = 1.0f;
	mat.w3_ = 1.0f;

	return mat;
}

//平行移動
Matrix4x4 Matrix4x4::Translate(float x, float y, float z)
{
	Matrix4x4 mat;

	mat.x0_ = 1.0f;
	mat.y1_ = 1.0f;
	mat.z2_ = 1.0f;
	mat.w3_ = 1.0f;

	mat.w0_ = x;
	mat.w1_ = y;
	mat.w2_ = z;

	return mat;
}

Matrix4x4 Matrix4x4::Scale(float x, float y, float z)
{
	Matrix4x4 mat;

	mat.x0_ = x;
	mat.y1_ = y;
	mat.z2_ = z;
	mat.w3_ = 1.0f;

	return mat;
}

Vector3 Matrix4x4::TransformForVector(const Vector3& vec) const
{
	return Vector3(
		vec.x_ * x0_ + vec.y_ * y0_ + vec.z_ * z0_ + w0_,
		vec.x_ * x1_ + vec.y_ * y1_ + vec.z_ * z1_ + w1_,
		vec.x_ * x2_ + vec.y_ * y2_ + vec.z_ * z2_ + w2_
	);
}

//行列をDxlib用に変換
MATRIX Matrix4x4::ToDxLibMatrix()
{
	MATRIX m{};

	m.m[0][0] = x0_; m.m[0][1] = x1_; m.m[0][2] = x2_; m.m[0][3] = x3_;
	m.m[1][0] = y0_; m.m[1][1] = y1_; m.m[1][2] = y2_; m.m[1][3] = y3_;
	m.m[2][0] = z0_; m.m[2][1] = z1_; m.m[2][2] = z2_; m.m[2][3] = z3_;
	m.m[3][0] = w0_; m.m[3][1] = w1_; m.m[3][2] = w2_; m.m[3][3] = w3_;

	return m;
}

