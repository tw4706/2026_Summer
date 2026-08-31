#include "BossHPGaugeUI.h"
#include "Game.h"
#include <Dxlib.h>

namespace
{
	//ボスHPUIの拡大率
	constexpr float kBossHPUIScale = 1.2f;

	//画面右上を基準にした描画位置のオフセット
	constexpr int kDrawOffsetX = 40;
	constexpr int kDrawOffsetY = 40;

	//HPUIのパス
	const wchar_t* kBossHPUI = L"data/UI/BossHP.png";
}

BossHPGaugeUI::BossHPGaugeUI(std::weak_ptr<EnemyBase> pEnemy) :
	EnemyHPGaugeUI(pEnemy)
{
}

void BossHPGaugeUI::Init()
{
	//基底(EnemyHPGaugeUI)の初期化を流用
	EnemyHPGaugeUI::Init();

	//ボスは大きめのスケールで上書き
	scale_ = kBossHPUIScale;
}

const wchar_t* BossHPGaugeUI::GetHPGraphPath() const
{
	return kBossHPUI;
}

bool BossHPGaugeUI::VisibleDrawPos(int& drawX, int& drawY) const
{
	//ワールド→スクリーン変換をせず、右上固定の座標を返す
	int scaledBarW = static_cast<int>(hpBarUIX_ * scale_);

	drawX = Game::kScreenWidth - scaledBarW - kDrawOffsetX;
	drawY = kDrawOffsetY;

	return true;
}