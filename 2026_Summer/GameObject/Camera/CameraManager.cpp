#include "CameraManager.h"
#include "CameraBase.h"

CameraManager::CameraManager():
	pActiveCamera_(nullptr)
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::RegisterCamera(const std::wstring& name, std::shared_ptr<CameraBase> camera)
{
	if (!camera) return;

	cameraMap_[name] = camera;

	//最初に登録されたカメラを自動的にアクティブにする
	if (!pActiveCamera_)
	{
		pActiveCamera_ = camera;
	}
}

void CameraManager::ChangeCamera(const std::wstring& name)
{
	//チェンジするカメラをマップから探す
	auto it = cameraMap_.find(name);
	if (it == cameraMap_.end())return;

	auto nextCamera = it->second;

	//アクティブなカメラが存在するなら
	if (pActiveCamera_)
	{
		//現在のカメラの座標とターゲット取得する
		Vector3 prevPos = pActiveCamera_->GetPos();
		Vector3 prevTarget = pActiveCamera_->GetCameraTarget();

		//今のカメラの座標とターゲットをそのまま次のカメラに適用する
		//これを行うことによって切り替わりの際にカメラがぶおんってならないようにする
		nextCamera->SetCompulsoryPosAndTarget(prevPos, prevTarget);
	}
	//アクティなカメラを切り替える
	pActiveCamera_ = nextCamera;

	//現在のカメラの名前を引数で来たカメラの名前に上書き
	currentCameraName_ = name;

	pActiveCamera_->Init();
}

void CameraManager::Update(int stageModelHandle)
{
	if (pActiveCamera_)
	{
		pActiveCamera_->Update(stageModelHandle);
	}
}

std::shared_ptr<CameraBase> CameraManager::GetCamera(const std::wstring& name)
{
	//引数で渡されたnameをfindで探す
	auto it = cameraMap_.find(name);

	//見つからない場合はnullptr
	if (it == cameraMap_.end())return nullptr;

	//要素の実体を返す
	return it->second;
}
