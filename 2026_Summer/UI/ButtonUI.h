#pragma once
#include "UIBase.h"
#include <vector>
class ButtonUI :public UIBase
{
public:
	ButtonUI();
	~ButtonUI();

	//静止画ボタンとして初期化
	//x,y : 描画位置　handle : 表示する画像ハンドル
	void Init(int x, int y, int handle, float scale);

	//アニメーションボタンとして初期化
	//x,y : 描画位置　handles : 切り替える画像ハンドル配列　interval : 切り替え間隔(フレーム数)
	void Init(int x, int y, const std::vector<int>& handles,float scale, int interval);

	void Init()override {};
	void Update()override;
	void Draw()override;
private:
	//アニメーションするかどうか
	bool isAnimation_ = false;

	//描画位置
	int drawX_ = 0;
	int drawY_ = 0;
	float scale_ = 1.0f;

	//ボタンUIハンドル(静止画用)
	int buttonUIHandle_ = -1;

	//ボタンUIハンドル配列(アニメーション用)
	std::vector<int> animeHandles_;
	//現在表示中のインデックス
	int animeIndex_ = 0;
	//切り替え間隔(フレーム数)
	int animeInterval_ = 1;
	//経過フレーム数
	int animeCounter_ = 0;
};

