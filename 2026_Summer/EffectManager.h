#pragma once
#include "Math/Vector3.h"
#include <map>
#include <string>
#include <vector>

class EffectManager
{
public:
	static EffectManager& GetInstance();

	void Update();

	/// <summary>
	/// エフェクトのロード
	/// </summary>
	/// <param name="name">エフェクトの文字列</param>
	/// <param name="path">エフェクトのパス</param>
	void Load(const std::wstring& name, const std::wstring& path);

	/// <summary>
	/// エフェクトの再生
	/// </summary>
	/// <param name="name">エフェクトの名前</param>
	/// <param name="pos">エフェクトを生成する座標</param>
	/// <returns>ハンドルを返す</returns>
	int Play(const std::wstring& name, const Vector3& pos);

	/// <summary>
	/// エフェクトが再生中かどうか
	/// </summary>
	/// <param name="handle">ハンドル</param>
	/// <returns>再生中ならtrue,そうでない場合はfalse</returns>
	bool IsPlaying(int handle)const;

	/// <summary>
	/// エフェクトの停止
	/// </summary>
	/// <param name="handle">ハンドル</param>
	void Stop(int handle);

	/// <summary>
	/// すべてのエフェクトの停止
	/// </summary>
	void StopAll();

	/// <summary>
	/// 使い終わったエフェクトの解放を行う
	/// </summary>
	void Clear();

private:
	//エフェクトハンドルを管理するマップ
	std::map<std::wstring, int>effects_;

	//エフェクトハンドルを管理する配列
	std::vector<int>handles_;
};

