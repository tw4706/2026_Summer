#pragma once
#include <memory>
#include <vector>

class Player;
class EnemyBase;
class CameraManager;
class Vector3;
class LockOnManager
{
public:
	LockOnManager();
	virtual~LockOnManager();

	void Update(std::shared_ptr<Player>pPlayer,const std::vector<std::shared_ptr<EnemyBase>>&pEnemies,
		CameraManager*pCameraManager);

	void Draw(std::shared_ptr<Player> pPlayer, CameraManager* pCameraManager);

	void StartLockOn(std::shared_ptr<Player>pPlayer, const std::vector<std::shared_ptr<EnemyBase>>& pEnemies,
		CameraManager* pCameraManager);


	/// <summary>
	/// ロックオンしている敵がいるかどうかの取得
	/// </summary>
	/// <returns>いるならtrue,いないならfalse</returns>
	bool IsLockOn()const { return pCurrentTarget_ != nullptr; }


private:
	std::shared_ptr<EnemyBase>pCurrentTarget_=nullptr;	//ターゲットになる敵
};

