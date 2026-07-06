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
	void RegisterCamera(const std::string& name, std::shared_ptr<CameraBase> camera);

	/// <summary>
	/// アクティブなカメラの切り替え
	/// </summary>
	void ChangeCamera(const std::string& name);

	/// <summary>
	/// 現在アクティブなカメラの更新
	/// </summary>
	void Update(int stageModelHandle);

	/// <summary>
	/// 現在アクティブなカメラを取得
	/// </summary>
	std::shared_ptr<CameraBase> GetActiveCamera() const { return pActiveCamera_; }

private:
	//登録されたカメラを名前で管理するマップ
	std::unordered_map<std::string, std::shared_ptr<CameraBase>> cameraMap_;

	//現在アクティブなカメラのポインタ
	std::shared_ptr<CameraBase> pActiveCamera_;
};

