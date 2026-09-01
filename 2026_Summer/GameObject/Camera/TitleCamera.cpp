#include "TitleCamera.h"
#include<Dxlib.h>

namespace
{
	//プレイヤーからのオフセット
	const Vector3 kFollowOffset = { 0.0f, 350.0f, 600.0f };

	//ロゴ表示時にさらに引き込む目標オフセット
	const Vector3 kLogoOffset = { 0.0f, 300.0f, 1000.0f };

	//注視点の高さオフセット
	const Vector3 kTargetHeightOffset = { 0.0f, 130.0f, 0.0f };

	//左斜め前の固定カメラオフセット
	const Vector3 kIntroOffset = { 200.0f, 300.0f, -1600.0f };

	//固定→追従へ引いていく際の補間割合
	constexpr float kPullBackLerpRate = 0.03f;

	//引きカメラの引くスピードの補間割合
	constexpr float kLogoPullBackLerpRate = 0.02f;

	//背後へカメラを引き込む時間
	constexpr int kTransitionDuration = 300;
}

TitleCamera::TitleCamera() :
	mode_(Mode::Follow)
{
}

TitleCamera::~TitleCamera()
{
}

void TitleCamera::Init()
{
	mode_ = Mode::FixedIntro;
}

void TitleCamera::Update(int stageModelHandle)
{
	CameraBase::Update(stageModelHandle);
}

void TitleCamera::FollowPlayer(const Vector3& playerPos)
{
	switch (mode_)
	{
	case Mode::FixedIntro:
		//プレイヤーだけが画面内を走っていく
		break;

	case Mode::TransitionToFollow:
	{
		//毎フレーム現在のプレイヤー位置の追従目標位置を計算して寄せる
		Vector3 desiredPos = playerPos + kFollowOffset;
		Vector3 desiredTarget = playerPos + kTargetHeightOffset;

		//補間計算でカメラ座標と注視点を更新
		pos_ = Vector3::Lerp(pos_, desiredPos, kPullBackLerpRate);
		cameraTarget_ = Vector3::Lerp(cameraTarget_, desiredTarget, kPullBackLerpRate);

		//時間遷移
		transitionFrameCount_++;
		if (transitionFrameCount_ >= kTransitionDuration)
		{
			//目標位置へ補正して追従モードに遷移
			pos_ = desiredPos;
			cameraTarget_ = desiredTarget;
			mode_ = Mode::Follow;
		}
		break;
	}

	case Mode::Follow:
		pos_ = playerPos + kFollowOffset;
		cameraTarget_ = playerPos + kTargetHeightOffset;
		break;
	case Mode::PullBackForLogo:
	{
		Vector3 desiredPos = playerPos + kLogoOffset;
		Vector3 desiredTarget = playerPos + kTargetHeightOffset;

		// スムーズに上・後ろへカメラを遠ざける
		pos_ = Vector3::Lerp(pos_, desiredPos, kLogoPullBackLerpRate);
		cameraTarget_ = Vector3::Lerp(cameraTarget_, desiredTarget, kLogoPullBackLerpRate);
		break;
	}
	}
}

void TitleCamera::SetupIntro(const Vector3& playerStartPos, const Vector3& moveDir)
{
	mode_ = Mode::FixedIntro;
	moveDir_ = moveDir;
	pos_ = playerStartPos + kIntroOffset;
	cameraTarget_ = playerStartPos + kTargetHeightOffset;
}

void TitleCamera::StartTransitionToFollow()
{
	if (mode_ == Mode::FixedIntro)
	{
		mode_ = Mode::TransitionToFollow;
	}
}

void TitleCamera::StartPullBackForLogo()
{
	mode_ = Mode::PullBackForLogo;
}

void TitleCamera::SkipFollow(const Vector3& playerPos)
{
	pos_ = playerPos + kLogoOffset;
	cameraTarget_ = playerPos + kTargetHeightOffset;
	mode_ = Mode::PullBackForLogo;
}
