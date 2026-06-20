#pragma once
#include"Matrix4x4.h"
class Model
{
public:
	Model();
	virtual~Model();

	void Load(const wchar_t* path);
	void Release();
	void Draw();

	/// <summary>
	/// 行列のセット
	/// </summary>
	/// <param name="mat">行列</param>
	void SetMatrix(const Matrix4x4& mat);

	/// <summary>
	/// 行列のセット(オーバーロード)
	/// </summary>
	/// <param name="mat">行列</param>
	void SetMatrix(const MATRIX& mat);

	/// <summary>
	/// フレームサーチ
	/// </summary>
	int SearchFrame(const wchar_t*frameName)const;

	/// <summary>
	/// ハンドルの取得
	/// </summary>
	/// <returns>モデルハンドル</returns>
	int GetHandle()const { return modelH_; }

private:
	//モデルハンドル
	int modelH_;
};

