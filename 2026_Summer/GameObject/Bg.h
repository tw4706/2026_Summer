#pragma once
#include"Math/Vector3.h"
#include<Dxlib.h>
#include<array>

class Bg
{
public:
	Bg();
	virtual~Bg();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cameraPos">カメラの位置</param>
	void Draw(const Vector3&cameraPos);
private:
	//スカイボックスのテクスチャハンドル
	std::array<int,6>skyTex_{};
};

