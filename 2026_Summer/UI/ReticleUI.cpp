#include "ReticleUI.h"
#include "Player/Player.h"
#include "Enemy/EnemyBase.h"
#include "Game.h"
#include <Dxlib.h>
#include <cassert>

namespace
{
	//レイ(視線)の高さ
	constexpr float kRayStartHeight = 80.0f;

	//レティクルの倍率
	constexpr float kReticleScale = 0.2f;

	//画面座標のX補正値
	constexpr float kReticleOffsetX = 100.0f;

	//画面座標のY補正値
	constexpr float kReticleOffsetY = 80.0f;

	//レティクル画像のパス
	const wchar_t* kReticleUIPath = L"data/UI/reticle.png";
}

ReticleUI::ReticleUI(std::weak_ptr<Player> pPlayer):
	UIBase(),
	pPlayer_(pPlayer),
	reticleHandle_(-1),
	reticleX_(0),
	reticleY_(0)
{
}

ReticleUI::~ReticleUI()
{
	DeleteGraph(reticleHandle_);
}

void ReticleUI::Init()
{
	//画像のロード
	reticleHandle_ = LoadGraph(kReticleUIPath);
	assert(reticleHandle_ >= 0);
	GetGraphSize(reticleHandle_, &reticleX_, &reticleY_);
}

void ReticleUI::Update()
{
}

void ReticleUI::Draw()
{
	auto pPlayer = pPlayer_.lock();
	if (!pPlayer) return;

	//ロックオンしていない場合は描画しない
	if (!pPlayer->IsLockOn()) return;

	auto target = pPlayer->GetLockOnEnemy().lock();
	if (!target) return;

	//始点
	Vector3 startPos = pPlayer->GetPos();
	startPos.y_ += kRayStartHeight;

	//終点(敵の目標座標)
	Vector3 endPos = target->GetCameraTarget();

	VECTOR start = startPos.ToDxlibVector();
	VECTOR end = endPos.ToDxlibVector();

	unsigned int rayColor = Game::kYellowColor;
	DrawLine3D(start, end, rayColor);

	VECTOR reticleScreenPos = ConvWorldPosToScreenPos(end);

	SetUseZBuffer3D(false);

	DrawRotaGraph3(static_cast<int>(reticleScreenPos.x - kReticleOffsetX), static_cast<int>(reticleScreenPos.y - kReticleOffsetY),
		0, 0,
		kReticleScale, kReticleScale,
		0.0f, reticleHandle_, true);

	SetUseZBuffer3D(true);
}
