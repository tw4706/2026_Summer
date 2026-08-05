#include "PathFollower.h"
#include<cmath>

namespace
{
	//目標点に到達したと判定する距離
	constexpr float kReachDistance = 50.0f;
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
	//経路データが存在するかつまだ最後まで進みきっていない時を返す
	return !path_.empty() && currentIndex_ < static_cast<int>(path_.size());
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

	//到達していたら次のwaypointへ進める
	while (currentIndex_<static_cast<int>(path_.size()))
	{
		//次のノードの座標に更新
		Vector3 targetPos = path_[currentIndex_];

		//現在の目標点との距離を計算
		Vector3 diff = targetPos - currentPos;
		diff.y_ = 0.0f;
		float dist = diff.LengthSq();

		//到達していない場合はその座標を目標とする
		if (dist >= kReachDistance * kReachDistance)
		{
			return targetPos;
		}

		//到達している場合はノードを進める
		currentIndex_++;
	}

	//すべてのノードを通っている場合は最終のノードの座標を返す
	return path_.back();
}
