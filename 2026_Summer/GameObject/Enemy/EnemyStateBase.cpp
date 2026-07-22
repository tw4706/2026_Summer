#include "EnemyStateBase.h"
#include "Enemy/EnemyBase.h"
#include "Math/Vector3.h"
#include "Camera/CameraBase.h"
#include "System/NavigationGrid.h"
#include "DxLib.h"
#include <cmath>

namespace
{
	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;
}

EnemyStateBase::EnemyStateBase(std::weak_ptr<EnemyBase> pEnemy,float searchRadius) :
	CharacterStateBase(pEnemy),
	searchRadius_(searchRadius),
	pEnemy_(pEnemy)
{
}

bool EnemyStateBase::PlayerSearchDistance(float radius) const
{
	//敵の参照
	auto enemy = pEnemy_.lock();

	//敵が存在しない場合はreturn
	if (!enemy) return false;

	//敵・プレイヤーそれぞれの位置を取得
	Vector3 enemyPos = enemy->GetPos();
	Vector3 playerPos = enemy->GetPlayerPos();

	//敵からプレイヤーへのベクトルを計算
	Vector3 toPlayer = playerPos - enemyPos;

	//距離を計算
	float distSq = toPlayer.LengthSq();

	float radiusSq = radius * radius;
	//Enemy->Playerの距離がEnemy+Playerより小さかったら検知
	return distSq <= radiusSq;
}

bool EnemyStateBase::HasLineOfSight(int stageModelHandle, const Vector3& from, const Vector3& to, float eyeHeight)
{
	VECTOR start = VGet(from.x_, from.y_ + eyeHeight, from.z_);
	VECTOR end = VGet(to.x_, to.y_ + eyeHeight, to.z_);

	MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageModelHandle, -1, start, end);

	return hit.HitFlag == false;
}

bool EnemyStateBase::IsPathWalkable(const NavigationGrid* pNaviGrid, const Vector3& from, const Vector3& to)
{
	//グリッドが無ければチェックできないので歩かせる
	if (!pNaviGrid) return true; 

	float cellSize = pNaviGrid->GetCellSize();
	if (cellSize <= 0.0f) return true;

	Vector3 diff = to - from;
	diff.y_ = 0.0f;
	float distance = diff.Length();

	//距離が短ければチェック不要
	if (distance < cellSize)
	{
		return true;
	}

	int sampleCount = static_cast<int>(distance / cellSize) + 1;

	for (int i = 0; i <= sampleCount; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(sampleCount);
		Vector3 samplePos = from + diff * t;

		int gx, gz;
		pNaviGrid->WorldPosToGrid(samplePos, gx, gz);

		const NavigationGrid::NodeData* node = pNaviGrid->GetNode(gx, gz);

		//グリッド範囲外または歩行不可なら直進不可と判定
		if (!node || !node->iswalked)
		{
			return false;
		}
	}

	return true;
}

float EnemyStateBase::RotateAngle(float currentAngle, const Vector3& toTargetDir, float lerpRate)
{
	//進行方向の角度
	float targetAngle = std::atan2f(toTargetDir.x_, -toTargetDir.z_);

	float angleDiff = targetAngle - currentAngle;

	//差分を-πからπの範囲に正規化
	while (angleDiff > DX_PI_F)  angleDiff -= 2.0f * DX_PI_F;
	while (angleDiff < -DX_PI_F) angleDiff += 2.0f * DX_PI_F;

	//線形補間を用いて滑らかに回転する
	return currentAngle + angleDiff * lerpRate;
}

void EnemyStateBase::ApplyMove(const std::shared_ptr<EnemyBase>& enemy, const Vector3& enemyPos, const Vector3& toTargetDir, float moveSpeed)
{
	if (!enemy) return;

	//移動速度を設定
	Vector3 moveVec = { toTargetDir.x_ * moveSpeed * kDeltaTime, 0.0f, toTargetDir.z_ * moveSpeed * kDeltaTime };

	//速度の適用
	enemy->SetVelocity(moveVec);

	//計算した位置を適用
	Vector3 nextPos = enemyPos + moveVec;
	enemy->SetPos(nextPos);
}

Vector3 EnemyStateBase::MoveTargetPath(const std::shared_ptr<EnemyBase>& enemy, const Vector3& startPos, const Vector3& endPos, bool& outHasLineOfSight)
{
	//目的地が存在しない場合はそのまま返す
	if (!enemy)
	{
		outHasLineOfSight = true;
		return endPos;
	}
	//目的地までの視線が通っているか判定
	outHasLineOfSight = HasLineOfSight(enemy->GetStageModelHandle(), startPos, endPos);
	if (outHasLineOfSight)
	{
		//障害物が無ければ経路探索は不要なのでクリアして直進する
		if (enemy->pathFollower_.HasPath())
		{
			enemy->pathFollower_.ClearPath();
		}
		return endPos;
	}
	//視線が通らない場合はA*で経路を探索する
	if (!enemy->pathFollower_.HasPath())
	{
		std::vector<Vector3> path = enemy->pathFinder_.FindPath(startPos, endPos);
		if (!path.empty())
		{
			enemy->pathFollower_.SetPath(path);
		}
	}
	//経路があればその中継地点へ無ければ直接目的地へ向かう
	if (enemy->pathFollower_.HasPath())
	{
		return enemy->pathFollower_.GetCurrentTarget(startPos);
	}
	return endPos;
}
