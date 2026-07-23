#include "EnemyHPGaugeUI.h"
#include "Enemy/EnemyBase.h"
#include <Dxlib.h>

namespace
{
	//HPUIの拡大率
	constexpr float kHPUIScale = 0.2f;

	//HPUIの見えている時間
	constexpr float kHPUIVisibleTime = 3.0f;

	//HPUIが消える時の条件の一つであるHPの基準
	constexpr float kDrawVisibleMinHP = 0.1f;

	constexpr float kDrawLerpRate = 0.1f;

	//頭上に表示するための座標のオフセット
	const Vector3 kDrawHeadOffset = { 0.0f,70.0f,0.0f };

	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//HPUIのパス
	const wchar_t* kEnemyHPUI = L"data/UI/EnemyHP.png";
}

EnemyHPGaugeUI::EnemyHPGaugeUI(std::weak_ptr<EnemyBase> pEnemy) :
	pEnemy_(pEnemy),
	isVisible_(false),
	visibleTimer_(0.0f)
{
}

void EnemyHPGaugeUI::Init()
{
	HPGaugeUI::Init();
	scale_ = kHPUIScale;

	auto enemy = pEnemy_.lock();
	if (!enemy) return;

	if (enemy)
	{
		maxHP_ = static_cast<float>(enemy->GetMaxHP());
		currentHP_ = static_cast<float>(enemy->GetHP());
	}
}

void EnemyHPGaugeUI::VisbleDamage()
{
	isVisible_ = true;
	visibleTimer_ = kHPUIVisibleTime;
}

const wchar_t* EnemyHPGaugeUI::GetHPGraphPath() const
{
	return kEnemyHPUI;
}

void EnemyHPGaugeUI::UpdateHP()
{
	auto enemy = pEnemy_.lock();
	if (!enemy) return;

	maxHP_ = static_cast<float>(enemy->GetMaxHP());

	float drawHP = static_cast<float>(enemy->GetHP());

	currentHP_ = currentHP_ + (drawHP - currentHP_) * kDrawLerpRate;

	if (isVisible_)
	{
		visibleTimer_ -= kDeltaTime;

		if (visibleTimer_ <= 0.0f && (maxHP_ - drawHP) < kDrawVisibleMinHP)
		{
			isVisible_ = false;
		}
	}
}

bool EnemyHPGaugeUI::VisibleDrawPos(int& drawX, int& drawY) const
{
	if (!isVisible_)return false;

	auto enemy = pEnemy_.lock();
	if (!enemy) return false;

	//敵の頭上の3D座標を計算
	Vector3 headWorldPos = enemy->GetCameraTarget() + kDrawHeadOffset;

	//D座標を画面の2D座標に変換
	VECTOR screenPos = ConvWorldPosToScreenPos(headWorldPos.ToDxlibVector());

	//画面外にいる場合は描画しない
	if (screenPos.z < 0.0f || screenPos.z > 1.0f)
	{
		return false;
	}

	int scaledBarW = static_cast<int>(hpBarUIX_ * scale_); //フレームの幅
	int scaledBarH = static_cast<int>(hpBarUIY_ * scale_); //フレームの高さ

	//HPバーの基準点を計算
	drawX = static_cast<int>(screenPos.x) - (scaledBarW / 2);
	drawY = static_cast<int>(screenPos.y) - (scaledBarH / 2);

	return true;

}
