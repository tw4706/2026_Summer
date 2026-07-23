#include "PlayerHPGaugeUI.h"
#include"Player/Player.h"

namespace
{
	//HPバーのオフセット
	constexpr int kHPBarOffsetX = 50;
	constexpr int kHPBarOffsetY = 50;

	//HPUIの拡大率
	constexpr float kHPUIScale = 0.8f;

	//HPUIのパス
	const wchar_t* kPlayerHPUI = L"data/UI/PlayerHP.png";
}

PlayerHPGaugeUI::PlayerHPGaugeUI(std::weak_ptr<Player> pPlayer) :
	pPlayer_(pPlayer)
{
}

PlayerHPGaugeUI::~PlayerHPGaugeUI()
{
}

void PlayerHPGaugeUI::Init()
{
	HPGaugeUI::Init();
	scale_ = kHPUIScale;
}

const wchar_t* PlayerHPGaugeUI::GetHPGraphPath() const
{
	return kPlayerHPUI;
}

void PlayerHPGaugeUI::UpdateHP()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	currentHP_ = static_cast<float>(pPlayer->GetHP());
	maxHP_ = static_cast<float>(pPlayer->GetMaxHP());
}

bool PlayerHPGaugeUI::VisibleDrawPos(int& drawX, int& drawY)const
{
	drawX = kHPBarOffsetX;
	drawY = kHPBarOffsetY;

	return true;
}