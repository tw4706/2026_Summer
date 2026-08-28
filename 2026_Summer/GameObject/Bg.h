#pragma once
#include"Math/Vector3.h"
#include<Dxlib.h>
#include<array>
#include<string>

class Bg
{
public:
	Bg();
	virtual~Bg();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(const std::wstring&folderPath);

	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cameraPos">カメラの位置</param>
	void Draw(const Vector3&cameraPos);
private:
	//Y軸の回転角度
	float rotationY_ = 0.0f;

	//スカイボックスのテクスチャハンドル
	std::array<int,6>skyTex_{};
};

