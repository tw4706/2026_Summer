#include "HPGaugeUI.h"
#include<Dxlib.h>
#include<cassert>

HPGaugeUI::HPGaugeUI():
	UIBase(),
	hpUIHandle_(-1),
	hpUIFrameHandle_(-1),
	hpUIX_(0),
	hpUIY_(0),
	hpBarUIX_(0),
	hpBarUIY_(0),
	currentHP_(0.0f),
	maxHP_(0.0f),
	scale_(1.0f)
{
}

HPGaugeUI::~HPGaugeUI()
{
	DeleteGraph(hpUIHandle_);
	DeleteGraph(hpUIFrameHandle_);
}

void HPGaugeUI::Init()
{
	//HPUI
	hpUIHandle_ = LoadGraph(GetHPGraphPath());
	assert(hpUIHandle_ >= 0);
	GetGraphSize(hpUIHandle_, &hpUIX_, &hpUIY_);

	//HPUIFrame
	hpUIFrameHandle_ = LoadGraph(L"data/UI/HPBar.png");
	assert(hpUIFrameHandle_ >= 0);
	GetGraphSize(hpUIFrameHandle_, &hpBarUIX_, &hpBarUIY_);
}

void HPGaugeUI::Update()
{
	UpdateHP();
}

void HPGaugeUI::Draw()
{
	int drawX = 0;
	int drawY = 0;

	if (!VisibleDrawPos(drawX, drawY))
	{
		return;
	}

	//HPの割合
	float hpRate = (maxHP_ > 0.0f) ? currentHP_ / maxHP_ : 0.0f;
	int drawHPWidth = static_cast<int>(hpUIX_ * hpRate);

	int scaledBarW = static_cast<int>(hpBarUIX_ * scale_); //フレームの幅
	int scaledBarH = static_cast<int>(hpBarUIY_ * scale_); //フレームの高さ
	int scaledHPW = static_cast<int>(drawHPWidth * scale_); //バーの幅

	//HPバーフレームの描画
	DrawRectExtendGraph(drawX, drawY,
		drawX + scaledBarW, drawY + scaledBarH,
		0, 0,
		hpBarUIX_, hpBarUIY_,
		hpUIFrameHandle_, true);

	//HPバーの描画
	DrawRectExtendGraph(drawX, drawY,
		drawX + scaledHPW, drawY + scaledBarH,
		0, 0,
		drawHPWidth, hpUIY_,
		hpUIHandle_, true);
}
