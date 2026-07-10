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
	constexpr float kMaxLockOnDistanceSq = 70.0f * 70.0f;

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
		Vector3 playerForward = pPlayer->GetCameraTarget();
		playerForward.y_ = 0.0f;
		playerForward = playerForward.Normalize();

		for (const auto& enemy : pEnemies)
		{
			if (!enemy || enemy->IsDead() || enemy == pCurrentTarget_) continue;

			Vector3 diff = enemy->GetPos() - playerPos;
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

void LockOnManager::StartLockOn(std::shared_ptr<Player> pPlayer, const std::vector<std::shared_ptr<EnemyBase>>& pEnemies, CameraManager* pCameraManager)
{
	if (!pPlayer || !pCameraManager) return;

	//すでにロックオン中なら解除する
	if (IsLockOn())
	{
		auto activeCam = pCameraManager->GetActiveCamera();
		auto playerCam = pCameraManager->GetCamera(L"PlayerCamera");
		auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(activeCam);
		auto playerCamera = std::dynamic_pointer_cast<PlayerCamera>(playerCam);

		// 通常カメラに戻る前に角度を同期
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
	Vector3 playerForward = pPlayer->GetCameraTarget();
	playerForward.y_ = 0.0f;
	playerForward = playerForward.Normalize();

	for (const auto& enemy : pEnemies)
	{
		if (!enemy || enemy->IsDead()) continue;

		Vector3 diff = enemy->GetPos() - playerPos;
		float distSq = diff.LengthSq();

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
