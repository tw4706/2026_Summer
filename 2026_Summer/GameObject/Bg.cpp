#include "Bg.h"
#include "Matrix4x4.h"
#include<Dxlib.h>

namespace
{
	//頂点のサイズ
	constexpr float kSize = 2000.0f;

	//回転速度
	constexpr float kRotationSpeed = 0.001f;
}

Bg::Bg()
{
}

Bg::~Bg()
{
	//ハンドルの削除
	DeleteGraph(skyTex_[0]);
	DeleteGraph(skyTex_[1]);
	DeleteGraph(skyTex_[2]);
	DeleteGraph(skyTex_[3]);
	DeleteGraph(skyTex_[4]);
	DeleteGraph(skyTex_[5]);
}

void Bg::Init(const std::wstring& folderPath)
{
	//スカイボックス用のテクスチャのロード
	skyTex_[0] = LoadGraph((folderPath + L"/rt.png").c_str());
	skyTex_[1] = LoadGraph((folderPath + L"/lf.png").c_str());
	skyTex_[2] = LoadGraph((folderPath + L"/up.png").c_str());
	skyTex_[3] = LoadGraph((folderPath + L"/dn.png").c_str());
	skyTex_[4] = LoadGraph((folderPath + L"/ft.png").c_str());
	skyTex_[5] = LoadGraph((folderPath + L"/bk.png").c_str());
}

void Bg::Update()
{
	//回転
	rotationY_ += kRotationSpeed;
}

void Bg::Draw(const Vector3& cameraPos)
{
	SetUseBackCulling(false);
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(false);
	SetUseLighting(false);

	VECTOR c = cameraPos.ToDxlibVector();

	//Y軸回転行列
	Matrix4x4 matRotY = Matrix4x4::RotateY(rotationY_);

	// 回転させてからカメラ位置を加算するラムダ式
	auto rotatedPos = [&](float ox, float oy, float oz) -> VECTOR
	{
		Vector3 offset(ox, oy, oz);
		Vector3 rotated = matRotY.TransformForVector(offset);
		return VGet(c.x + rotated.x_, c.y + rotated.y_, c.z + rotated.z_);
	};

	// =========================
	// 各面の4頂点生成関数のように書く
	// =========================
	auto drawFace = [&](VECTOR v0, VECTOR v1, VECTOR v2, VECTOR v3, int tex)
	{
			VERTEX3D v[4];
			v[0].pos = v0;
			v[1].pos = v1;
			v[2].pos = v2;
			v[3].pos = v3;
			for (int i = 0; i < 4; i++)
			{
				v[i].dif = GetColorU8(255, 255, 255, 255);
				v[i].spc = GetColorU8(0, 0, 0, 0);
			}
			v[0].u = 0; v[0].v = 1;
			v[1].u = 1; v[1].v = 1;
			v[2].u = 0; v[2].v = 0;
			v[3].u = 1; v[3].v = 0;
			DrawPrimitive3D(v, 4, DX_PRIMTYPE_TRIANGLESTRIP, tex, false);
	};

	// 前面
	drawFace(
		rotatedPos(-kSize, -kSize, +kSize),
		rotatedPos(+kSize, -kSize, +kSize),
		rotatedPos(-kSize, +kSize, +kSize),
		rotatedPos(+kSize, +kSize, +kSize),
		skyTex_[4]);
	// 背面
	drawFace(
		rotatedPos(+kSize, -kSize, -kSize),
		rotatedPos(-kSize, -kSize, -kSize),
		rotatedPos(+kSize, +kSize, -kSize),
		rotatedPos(-kSize, +kSize, -kSize),
		skyTex_[5]);
	// 左面
	drawFace(
		rotatedPos(-kSize, -kSize, -kSize),
		rotatedPos(-kSize, -kSize, +kSize),
		rotatedPos(-kSize, +kSize, -kSize),
		rotatedPos(-kSize, +kSize, +kSize),
		skyTex_[0]);
	// 右面
	drawFace(
		rotatedPos(+kSize, -kSize, +kSize),
		rotatedPos(+kSize, -kSize, -kSize),
		rotatedPos(+kSize, +kSize, +kSize),
		rotatedPos(+kSize, +kSize, -kSize),
		skyTex_[1]);
	// 上面
	drawFace(
		rotatedPos(-kSize, +kSize, -kSize),
		rotatedPos(-kSize, +kSize, +kSize),
		rotatedPos(+kSize, +kSize, -kSize),
		rotatedPos(+kSize, +kSize, +kSize),
		skyTex_[2]);
	// 下面
	drawFace(
		rotatedPos(-kSize, -kSize, -kSize),
		rotatedPos(+kSize, -kSize, -kSize),
		rotatedPos(-kSize, -kSize, +kSize),
		rotatedPos(+kSize, -kSize, +kSize),
		skyTex_[3]);

	SetUseZBuffer3D(true);
	SetUseLighting(true);
	SetUseBackCulling(true);
}
