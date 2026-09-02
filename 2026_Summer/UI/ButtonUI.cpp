#include "ButtonUI.h"
#include<Dxlib.h>

ButtonUI::ButtonUI():
	UIBase(),
	buttonUIHandle_(-1)
{
}

ButtonUI::~ButtonUI()
{
	//画像の削除
	DeleteGraph(buttonUIHandle_);
}


void ButtonUI::Init(int x, int y, int handle,float scale)
{
	isAnimation_ = false;
	drawX_ = x;
	drawY_ = y;
	buttonUIHandle_ = handle;
	scale_ = scale;
}
void ButtonUI::Init(int x, int y, const std::vector<int>& handles, float scale, int interval)
{
	isAnimation_ = true;
	drawX_ = x;
	drawY_ = y;
	animeHandles_ = handles;
	scale_ = scale;
	animeInterval_ = interval;
	animeIndex_ = 0;
	animeCounter_ = 0;
}

void ButtonUI::Update()
{
	//アニメーションしなければ何もしない
	if (!isAnimation_|| animeHandles_.empty())
	{
		return;
	}

	animeCounter_++;
	if (animeCounter_ >= animeInterval_)
	{
		animeCounter_ = 0;
		animeIndex_++;
		if (animeIndex_ >= static_cast<int>(animeHandles_.size()))
		{
			animeIndex_ = 0;
		}
	}
}
void ButtonUI::Draw()
{
	//描画するハンドルがアニメーションするかどうかを判定
	int handle = isAnimation_ ? (animeHandles_.empty() ? -1 : animeHandles_[animeIndex_]) : buttonUIHandle_;

	if (handle == -1)
	{
		return;
	}

	//1.0fならそのまま描画
	if (scale_ == 1.0f)
	{
		DrawGraph(drawX_, drawY_, handle, TRUE);
		return;
	}

	//元画像サイズを取得して倍率サイズで描画
	int width, height;
	GetGraphSize(handle, &width, &height);
	int destWidth = static_cast<int>(width * scale_);
	int destHeight = static_cast<int>(height * scale_);

	DrawExtendGraph(drawX_, drawY_, drawX_ + destWidth, drawY_ + destHeight, handle, TRUE);
}