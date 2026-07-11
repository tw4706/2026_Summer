#include "LockOnManager.h"
#include "Player/Player.h"
#include "Enemy/EnemyBase.h"
#include "Camera/CameraManager.h"
#include "Camera/LockOnCamera.h"
#include "Camera/PlayerCamera.h"
#include "Input.h"
#include <cmath>

namespace
{
	//ロックオンで探す最大距離
	constexpr float kMaxLockOnDistance = 1500.0f;
	constexpr float kMaxLockOnDistanceSq = kMaxLockOnDistance * kMaxLockOnDistance;

	//ターゲット切り替え時に敵を切り替えれる距離
	constexpr float kTargetChangeDistance = 3000.0f;
	constexpr float kTargetChangeDistanceSq = kTargetChangeDistance * kTargetChangeDistance;

	//ロックオンを行う索敵の視野角
	constexpr float kLockOnAngle = 0.707f;
}

LockOnManager::LockOnManager()
{
}

LockOnManager::~LockOnManager()
{
}

void LockOnManager::Update(std::shared_ptr<Player> pPlayer, const std::vector<std::shared_ptr<EnemyBase>>& pEnemies, CameraManager* pCameraManager)
{
	if (!pPlayer || !pCameraManager) return;

	//ロックオン中なら何もしない
	if (!IsLockOn()) return;

	//ターゲットが死んでいるとき
	if (pCurrentTarget_->IsDead())
	{
		//次のターゲットを距離を見て決める
		float closestDistanceSq = FLT_MAX;
		std::shared_ptr<EnemyBase> nextEnemy = nullptr;

		Vector3 playerPos = pPlayer->GetPos();
		Vector3 playerForward = pPlayer->GetCameraTarget() - playerPos;
		playerForward.y_ = 0.0f;
		playerForward = playerForward.Normalize();

		for (const auto& enemy : pEnemies)
		{
			if (!enemy || enemy->IsDead() || enemy == pCurrentTarget_) continue;

			//プレイヤーとの距離が1000以上離れている敵は無視
			Vector3 diff = enemy->GetPos() - playerPos;
			if (diff.LengthSq() > kTargetChangeDistanceSq) continue;

			float distSq = diff.LengthSq();
			if (distSq > kMaxLockOnDistanceSq) continue;

			Vector3 toEnemyDir = diff;
			toEnemyDir.y_ = 0.0f;
			toEnemyDir = toEnemyDir.Normalize();

			float dotResult = playerForward.Dot(toEnemyDir);
			if (dotResult < kLockOnAngle) continue;

			if (distSq < closestDistanceSq)
			{
				closestDistanceSq = distSq;
				nextEnemy = enemy;
			}
		}

		//次の敵が見つかれば切り替え、いなければロックオンを解除
		if (nextEnemy)
		{
			auto camera = pCameraManager->GetCamera(L"LockOnCamera");
			auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(camera);
			if (lockOnCamera)
			{
				lockOnCamera->SetTargetEnemy(nextEnemy);
				pPlayer->SetLockOnEnemy(nextEnemy);
				pCurrentTarget_ = nextEnemy;
			}
		}
		else
		{
			//敵が全滅、または範囲内に誰もいないならロックオンを解除
			pCameraManager->ChangeCamera(L"PlayerCamera");
			pPlayer->SetLockOn(false);
			pCurrentTarget_ = nullptr;
			return;
		}
	}

	//右スティックのフリック入力を検知した際のターゲット切り替え
	if (Input::GetInstance().IsRightStickFlickLeft() || Input::GetInstance().IsRightStickFlickRight())
	{
		Vector3 playerPos = pPlayer->GetPos();

		//現在のターゲットへのベクトル
		Vector3 vecA = pCurrentTarget_->GetPos() - playerPos;
		vecA.y_ = 0.0f;
		vecA = vecA.Normalize();

		std::shared_ptr<EnemyBase> nextTarget = nullptr;
		float maxDot = -2.0f;

		bool isFlickRight = Input::GetInstance().IsRightStickFlickRight();

		for (const auto& enemy : pEnemies)
		{
			if (!enemy || enemy->IsDead() || enemy == pCurrentTarget_) continue;

			Vector3 vecB = enemy->GetPos() - playerPos;
			if (vecB.LengthSq() > kTargetChangeDistanceSq) continue;

			vecB.y_ = 0.0f;
			vecB = vecB.Normalize();

			//外積で左右判定
			Vector3 crossResult = vecA.Cross(vecB);
			bool isEnemyOnRight = (crossResult.y_ < 0.0f);

			if ((isFlickRight && isEnemyOnRight) || (!isFlickRight && !isEnemyOnRight))
			{
				//現在のターゲットと角度が一番近い敵を選ぶ
				float dotResult = vecA.Dot(vecB);
				if (dotResult > maxDot)
				{
					maxDot = dotResult;
					nextTarget = enemy;
				}
			}
		}

		//新しいターゲットが見つかったら更新
		if (nextTarget)
		{
			auto camera = pCameraManager->GetCamera(L"LockOnCamera");
			auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(camera);
			if (lockOnCamera)
			{
				lockOnCamera->SetTargetEnemy(nextTarget);
				pPlayer->SetLockOnEnemy(nextTarget);
				pCurrentTarget_ = nextTarget;
			}
		}
	}
}

void LockOnManager::Draw(std::shared_ptr<Player> pPlayer, CameraManager* pCameraManager)
{
#ifdef _DEBUG
	auto activeCam = pCameraManager->GetActiveCamera();
	Vector3 cameraPos = activeCam->GetPos();

	//開始座標(始点)
	Vector3 startPos = pPlayer->GetPos();
	//プレイヤーの正面のベクトル
	Vector3 playerForward = pPlayer->GetForWardVec();
	playerForward.y_ = 0.0f;
	playerForward.Normalize();//正規化

	Vector3 centerEndPos = startPos + playerForward * kMaxLockOnDistance;
	Vector3 leftDir = { playerForward.x_ * kLockOnAngle - playerForward.z_ * kLockOnAngle ,0.0f, playerForward.z_ * kLockOnAngle + playerForward.x_ * kLockOnAngle };
	Vector3 rightDir = { playerForward.x_ * kLockOnAngle + playerForward.z_ * kLockOnAngle ,0.0f, -playerForward.x_ * kLockOnAngle + playerForward.z_ * kLockOnAngle };

	Vector3 leftEndPos = startPos + leftDir * kMaxLockOnDistance;
	Vector3 rightEndPos = startPos + rightDir * kMaxLockOnDistance;

	//1本目
	DrawLine3D(startPos.ToDxlibVector(), centerEndPos.ToDxlibVector(), GetColor(255, 0, 0));
	//2本目
	DrawLine3D(startPos.ToDxlibVector(), leftEndPos.ToDxlibVector(), GetColor(255, 0, 0));
	//3本目
	DrawLine3D(startPos.ToDxlibVector(), rightEndPos.ToDxlibVector(), GetColor(255, 0, 0));
#endif
}

void LockOnManager::StartLockOn(std::shared_ptr<Player> pPlayer, const std::vector<std::shared_ptr<EnemyBase>>& pEnemies, CameraManager* pCameraManager)
{
	if (!pPlayer || !pCameraManager) return;

	//すでにロックオン中なら
	if (IsLockOn())
	{
		auto activeCam = pCameraManager->GetActiveCamera();
		auto playerCam = pCameraManager->GetCamera(L"PlayerCamera");
		auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(activeCam);
		auto playerCamera = std::dynamic_pointer_cast<PlayerCamera>(playerCam);

		//プレイヤーカメラに戻る前に角度を設定
		if (lockOnCamera && playerCamera)
		{
			playerCamera->SetRotationToLockOn(lockOnCamera->GetPos(), lockOnCamera->GetCameraTarget());
		}

		pCameraManager->ChangeCamera(L"PlayerCamera");
		pPlayer->SetLockOn(false);
		pCurrentTarget_ = nullptr;
		return;
	}

	//一番近い敵を探す
	float closestDistanceSq = FLT_MAX;
	std::shared_ptr<EnemyBase> closestEnemy = nullptr;

	Vector3 playerPos = pPlayer->GetPos();
	Vector3 playerForward = pPlayer->GetForWardVec();
	playerForward.y_ = 0.0f;
	playerForward = playerForward.Normalize();

	for (const auto& enemy : pEnemies)
	{
		if (!enemy || enemy->IsDead()) continue;

		Vector3 diff = enemy->GetPos() - playerPos;
		float distSq = diff.LengthSq();
		float dist = std::sqrtf(distSq);

#ifdef _DEBUG
		OutputDebugStringA(("dist: " + std::to_string(dist) + "\n").c_str());
#endif

		//距離制限
		if (distSq > kMaxLockOnDistanceSq) continue;

		//角度制限（内積）
		Vector3 toEnemyDir = diff;
		toEnemyDir.y_ = 0.0f;
		toEnemyDir = toEnemyDir.Normalize();

		float dotResult = playerForward.Dot(toEnemyDir);

		if (dotResult < kLockOnAngle) continue; //視野外なら無視

		//条件を満たした中で最至近を更新
		if (distSq < closestDistanceSq)
		{
			closestDistanceSq = distSq;
			closestEnemy = enemy;
		}
	}

	// ターゲットが見つかったらロックオン開始
	if (closestEnemy)
	{
		//各カメラの取得
		auto camera = pCameraManager->GetCamera(L"LockOnCamera");
		auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(camera);

		//ロックオンカメラが存在する場合
		if (lockOnCamera)
		{
			//ターゲットのセット(敵)
			lockOnCamera->SetTargetEnemy(closestEnemy);
			//プレイヤーのセット
			lockOnCamera->SetPlayer(pPlayer);

			//カメラの切り替え
			pCameraManager->ChangeCamera(L"LockOnCamera");
			pPlayer->SetLockOn(true);
			pPlayer->SetLockOnEnemy(closestEnemy);
			pCurrentTarget_ = closestEnemy;
		}
	}
}
