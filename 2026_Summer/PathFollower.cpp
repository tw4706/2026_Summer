#include "PathFollower.h"
#include<cmath>

namespace
{
	//目標点に到達したと判定する距離
	const float kReachDistance = 30.0f;
}

PathFollower::PathFollower()
{
}

void PathFollower::SetPath(const std::vector<Vector3>& path)
{
	path_ = path;
	currentIndex_ = 0;
}

void PathFollower::ClearPath()
{
	path_.clear();
	currentIndex_ = 0;
}

bool PathFollower::HasPath() const
{
	return !path_.empty();
}

bool PathFollower::IsPathFinished() const
{
	if (path_.empty())
	{
		return true;
	}
	//進み切っているかどうか
	return currentIndex_ >= static_cast<int>(path_.size());
}

Vector3 PathFollower::GetCurrentTarget(const Vector3& currentPos)
{
	//経路がない、または進み切っている場合は現在地をそのまま返す
	if (path_.empty() || currentIndex_ >= static_cast<int>(path_.size()))
	{
		return currentPos;
	}

	Vector3 targetPos = path_[currentIndex_];

	//現在の目標点との距離を計算
	Vector3 diff = targetPos - currentPos;
	diff.y_ = 0.0f;
	float dist = diff.LengthSq();

	//到達していたら次のwaypointへ進める
	if (dist < kReachDistance * kReachDistance)
	{
		currentIndex_++;

		//最後まで行ったら現在の位置を返して終了
		if (currentIndex_ >= static_cast<int>(path_.size()))
		{
			return targetPos; // 最後のノード位置を返す
		}
		// 次のノードの座標に更新
		targetPos = path_[currentIndex_];
	}

	return targetPos;
}
