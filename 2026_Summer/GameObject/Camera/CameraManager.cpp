#include "CameraManager.h"
#include "CameraBase.h"

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

	//最初に登録されたカメラを自動的にアクティブにする
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

		//カメラが切り替わった瞬間に初期位置を計算し直す
		pActiveCamera_->Init();
	}
}

void CameraManager::Update()
{
	if (pActiveCamera_)
	{
		pActiveCamera_->Update();
	}
}
