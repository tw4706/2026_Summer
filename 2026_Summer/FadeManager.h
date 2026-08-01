#pragma once
class FadeManager
{
public:
	static FadeManager& GetInstance();

	FadeManager(const FadeManager&) = delete;
	FadeManager& operator=(const FadeManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Release();

	/// <summary>
	/// シーンの描画内容をキャプチャする描画先に切り替える
	/// </summary>
	void StartCapture();

	/// <summary>
	/// キャプチャした内容を元の描画先に戻しディゾルブシェーダで合成描画
	/// </summary>
	/// <param name="rate">フェードの進行度</param>
	void EndCaptureAndDraw(float rate);

private:
	FadeManager() = default;
	~FadeManager() = default;

	//シェーダー用の定数バッファ
	struct ConstantBuffer
	{
		float value;
		float strength;
		float lightX;
		float lightY;
	};

	//シーンの描画結果をキャプチャするレンダーターゲット
	int renderHandle_ = -1;

	//ディゾルブ用ノイズ画像
	int noiseHandle_ = -1;

	//ディゾルブ用ピクセルシェーダ
	int dissolvePSHandle_ = -1;

	//シェーダー定数バッファ
	int cBuffH_ = -1;

	ConstantBuffer* pCBuff_ = nullptr;

	//キャプチャに切り替える前の描画先
	int prevScreen_ = -1;

	bool isInitialized_ = false;
};

