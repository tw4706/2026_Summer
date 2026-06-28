#pragma once
#include "Math/Vector3.h"
#include <vector>

class PathFollower
{
public:
	PathFollower();

	/// <summary>
	/// 経路のセット
	/// </summary>
	/// <param name="path">経路</param>
	void SetPath(const std::vector<Vector3>&path);

	/// <summary>
	/// 経路のクリア
	/// </summary>
	void ClearPath();

	/// <summary>
	///	現在経路を持っているかどうか
	/// </summary>
	/// <returns>trueなら持っている,falseなら持っていない</returns>
	bool HasPath()const;

	/// <summary>
	/// 経路を進み終えたかどうか
	/// </summary>
	/// <returns>完了していればtrue、そうでない場合はfalseを返す</returns>
	bool IsPathFinished()const;

	/// <summary>
	/// 現在の位置からターゲットとなる座標の取得
	/// </summary>
	/// <param name="currentPos">現在の位置を表すベクトル</param>
	/// <returns>ターゲットとなる座標</returns>
	Vector3 GetCurrentTarget(const Vector3& currentPos);

private:
	std::vector<Vector3> path_;
	int currentIndex_ = 0;
};

