#pragma once
#include <memory>
#include <unordered_map>
#include <string>

class CameraBase;
class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	/// <summary>
	/// カメラの登録
	/// </summary>
	void RegisterCamera(const std::wstring& name, std::shared_ptr<CameraBase> camera);

	/// <summary>
	/// アクティブなカメラの切り替え
	/// </summary>
	void ChangeCamera(const std::wstring& name);

	/// <summary>
	/// 現在アクティブなカメラの更新
	/// </summary>
	void Update(int stageModelHandle);

	/// <summary>
	/// 現在アクティブなカメラを取得
	/// </summary>
	std::shared_ptr<CameraBase> GetActiveCamera() const { return pActiveCamera_; }

	std::shared_ptr<CameraBase>GetCamera(const std::wstring& name);

	/// <summary>
	/// 現在アクティブなカメラの名前を取得
	/// </summary>
	/// <returns>アクティブなカメラの名前を返す</returns>
	const std::wstring& GetActiveCameraName()const {return currentCameraName_;}

private:
	//登録されたカメラを名前で管理するマップ
	std::unordered_map<std::wstring, std::shared_ptr<CameraBase>> cameraMap_;

	//現在アクティブなカメラのポインタ
	std::shared_ptr<CameraBase> pActiveCamera_;

	//現在のカメラの名前
	std::wstring currentCameraName_;
};

