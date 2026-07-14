#pragma once
class Application
{
	Application();
	Application(const Application& app) = delete;
	void operator=(const Application& app) = delete;

public:
	virtual~Application();

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Application& GetInstance();

	bool Init();
	void Run();
	void Terminate();

	/// <summary>
	/// ゲームを終了する関数
	/// </summary>
	void GameEnd() { isGameEnd_ = true; }

private:
	bool isGameEnd_=false;	//ゲームを終了するかどうかのフラグ
};

