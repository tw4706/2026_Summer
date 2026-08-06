#include "Bg.h"
#include<Dxlib.h>

namespace
{
	//頂点のサイズ
	constexpr float kSize = 2000.0f;
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

void Bg::Init()
{
	//スカイボックス用のテクスチャのロード
	skyTex_[0] = LoadGraph(L"data/Bg/backGround_rt.png");
	skyTex_[1] = LoadGraph(L"data/Bg/backGround_lf.png");
	skyTex_[2] = LoadGraph(L"data/Bg/backGround_up.png");
	skyTex_[3] = LoadGraph(L"data/Bg/backGround_dn.png");
	skyTex_[4] = LoadGraph(L"data/Bg/backGround_ft.png");
	skyTex_[5] = LoadGraph(L"data/Bg/backGround_bk.png");
}

void Bg::Draw(const Vector3& cameraPos)
{
	SetUseBackCulling(false);
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(false);
	SetUseLighting(false);

	VECTOR c = cameraPos.ToDxlibVector();

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

			//完全リセット
			for (int i = 0; i < 4; i++)
			{
				v[i].dif = GetColorU8(255, 255, 255, 255);	//色そのまま
				v[i].spc = GetColorU8(0, 0, 0, 0);			//反射消す
			}

			//UV固定
			v[0].u = 0; v[0].v = 1;
			v[1].u = 1; v[1].v = 1;
			v[2].u = 0; v[2].v = 0;
			v[3].u = 1; v[3].v = 0;

			DrawPrimitive3D(v, 4, DX_PRIMTYPE_TRIANGLESTRIP, tex, false);
		};

	// =========================
	// 前面
	// =========================
	drawFace(
		VGet(c.x - kSize, c.y - kSize, c.z + kSize),
		VGet(c.x + kSize, c.y - kSize, c.z + kSize),
		VGet(c.x - kSize, c.y + kSize, c.z + kSize),
		VGet(c.x + kSize, c.y + kSize, c.z + kSize),
		skyTex_[4]);

	// =========================
	// 背面
	// =========================
	drawFace(
		VGet(c.x + kSize, c.y - kSize, c.z - kSize),
		VGet(c.x - kSize, c.y - kSize, c.z - kSize),
		VGet(c.x + kSize, c.y + kSize, c.z - kSize),
		VGet(c.x - kSize, c.y + kSize, c.z - kSize),
		skyTex_[5]);

	// =========================
	// 左面
	// =========================
	drawFace(
		VGet(c.x - kSize, c.y - kSize, c.z - kSize),
		VGet(c.x - kSize, c.y - kSize, c.z + kSize),
		VGet(c.x - kSize, c.y + kSize, c.z - kSize),
		VGet(c.x - kSize, c.y + kSize, c.z + kSize),
		skyTex_[0]);

	// =========================
	// 右面
	// =========================
	drawFace(
		VGet(c.x + kSize, c.y - kSize, c.z + kSize),
		VGet(c.x + kSize, c.y - kSize, c.z - kSize),
		VGet(c.x + kSize, c.y + kSize, c.z + kSize),
		VGet(c.x + kSize, c.y + kSize, c.z - kSize),
		skyTex_[1]);

	// =========================
	// 上面
	// =========================
	drawFace(
		VGet(c.x - kSize, c.y + kSize, c.z - kSize),
		VGet(c.x - kSize, c.y + kSize, c.z + kSize),
		VGet(c.x + kSize, c.y + kSize, c.z - kSize),
		VGet(c.x + kSize, c.y + kSize, c.z + kSize),
		skyTex_[2]);

	// =========================
	// 下
	// =========================
	drawFace(
		VGet(c.x - kSize, c.y - kSize, c.z - kSize),
		VGet(c.x + kSize, c.y - kSize, c.z - kSize),
		VGet(c.x - kSize, c.y - kSize, c.z + kSize),
		VGet(c.x + kSize, c.y - kSize, c.z + kSize),
		skyTex_[3]);

	SetUseZBuffer3D(true);
	SetUseLighting(true);
	SetUseBackCulling(true);
}
