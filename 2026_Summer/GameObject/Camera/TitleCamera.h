#pragma once
#include "CameraBase.h"
class TitleCamera :public CameraBase
{
public:
	TitleCamera();
	~TitleCamera() override;

	void Init() override;
	void Update(int stageModelHandle) override;

	//プレイヤー追従
	void FollowPlayer(const Vector3& playerPos);

	//イントロ用の固定カメラ設置
	void SetupIntro(const Vector3& playerStartPos, const Vector3& moveDir);

	//固定演出から追従への遷移を開始する
	void StartTransitionToFollow();

	//ロゴ表示時の引きカメラ開始
	void StartPullBackForLogo();

	//追従状態に切り替わったかどうか
	bool IsFollowing() const { return mode_ == Mode::Follow; }

	/// <summary>
	/// 演出をスキップする関数
	/// </summary>
	/// <param name="playerPos">タイトル用プレイヤーの座標</param>
	void SkipFollow(const Vector3& playerPos);
private:
	enum class Mode
	{
		FixedIntro,			//左斜め前に固定
		TransitionToFollow,	//追従位置へ徐々に移動
		Follow,				//背後に追従
		PullBackForLogo
	};

	int transitionFrameCount_ = 0;//カメラの遷移フレームカウンタ

	Mode mode_;
	Vector3 moveDir_;
};

