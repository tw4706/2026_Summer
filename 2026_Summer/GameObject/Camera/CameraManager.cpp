#include "CameraManager.h"
#include "CameraBase.h"
#include "PlayerCamera.h"

CameraManager::CameraManager():
	pActiveCamera_(nullptr)
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::RegisterCamera(const std::string& name, std::shared_ptr<CameraBase> camera)
{
	if (!camera) return;

	cameraMap_[name] = camera;

	//最初に登録されたカメラをアクティブにする
	if (!pActiveCamera_)
	{
		pActiveCamera_ = camera;
	}
}

void CameraManager::ChangeCamera(const std::string& name)
{
	auto it = cameraMap_.find(name);
	if (it != cameraMap_.end())
	{
		pActiveCamera_ = it->second;

		//カメラが切り替わった瞬間に初期位置を計算
		pActiveCamera_->Init();
	}
}

void CameraManager::Update(Input&input)
{
	if (pActiveCamera_)
	{
		//プレイヤーカメラの場合は引数に入力を受け取る必要があるため別で更新
		auto playerCamera = std::dynamic_pointer_cast<PlayerCamera>(pActiveCamera_);
		if (playerCamera)
		{
			playerCamera->Update(input);
		}
		else
		{
			//そうでない場合は引数なしのカメラの更新
			pActiveCamera_->Update();
		}
	}
}
