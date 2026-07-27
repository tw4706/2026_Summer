#include "Animation.h"
#include<Dxlib.h>
#include <cmath>

namespace
{
	//アニメーションの進むスピード
	constexpr float kAnimationSpeed = 30.0f;

	//スローアニメーションのスピード
	constexpr float kSlowAnimationSpeed = 24.0f;

	//敵の攻撃アニメーションのスピード
	constexpr float kEnemyAttackAnimationSpeed = 60.0f;

	//アニメーションをブレンドする割合
	constexpr float kBlendDuration = 0.3f;

	//アニメーションのスピード
	constexpr float kSpeed = 1.0f;

	//ブレンド率
	constexpr float kBlendRate1 = 1.0f;
	constexpr float kBlendRate0 = 0.0f;
}

Animation::Animation() :
	modelHandle_(-1),
	currentAttachAnim_(-1),
	currentAnim_(-1),
	currentTime_(0.0f),
	prevAttach_(-1),
	blendTime_(0.0f),
	blendDuration_(0.0f),
	isBlending_(false),
	speed_(0.0f),
	isLoop_(true),
	isAnimEnd_(false),
	totalTime_(0.0f),
	state_(AnimationState::None),
	prevState_(AnimationState::None),
	currentAnimName_(L"")
{

}

Animation::~Animation()
{
}

void Animation::Init(int modelHandle)
{
	modelHandle_ = modelHandle;
	blendDuration_ = kBlendDuration;
	speed_ = kSpeed;
}

void Animation::Update(float deltaTime)
{
	if (currentAttachAnim_ != -1 && totalTime_ == 0.0f)
	{
		totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttachAnim_);
	}

	//アニメーションの更新
	if (currentAttachAnim_ != -1)
	{
		//アニメーションの時間を進行
		currentTime_ += deltaTime * speed_;

		//アニメーションのループ
		if (totalTime_ > 0.0f)
		{
			//ループするなら
			if (isLoop_)
			{
				//アニメーションの時間がトータルタイムを超えたらfmodで余りを求めてループさせる
				if (currentTime_ > totalTime_)
				{
					currentTime_ = fmod(currentTime_, totalTime_);
				}
			}
			//ループしないなら
			else
			{
				//アニメーションの時間がトータルタイムを超えたらトータルタイムで止める
				if (currentTime_ > totalTime_)
				{
					currentTime_ = totalTime_;
					isAnimEnd_ = true;
				}
			}
		}

		//アニメーションの時間をセット
		MV1SetAttachAnimTime(modelHandle_, currentAttachAnim_, currentTime_);

		//旧アニメーションも同様に時間を進行させる(ブレンド中は新旧両方のアニメーションが再生されるため)
		if (prevAttach_ != -1)
		{
			float prevTime = MV1GetAttachAnimTime(modelHandle_, prevAttach_);
			float prevTotal = MV1GetAttachAnimTotalTime(modelHandle_, prevAttach_);

			prevTime += deltaTime * speed_;

			if (prevTotal > 0.0f)
			{
				if (isLoop_)
				{
					if (prevTime > prevTotal)
					{
						prevTime = fmod(prevTime, prevTotal);
					}
				}
				else
				{
					if (prevTime > prevTotal)
					{
						prevTime = prevTotal;
					}
				}
			}

			MV1SetAttachAnimTime(modelHandle_, prevAttach_, prevTime);
		}
	}
	//アニメーションのブレンド
	if (isBlending_)
	{
		//ブレンドタイムを進行
		blendTime_ += deltaTime;

		float t = kBlendRate1;

		//ブレンドタイムがDurationを超えないように0.0f~1.0fで正規化する
		if (blendDuration_ > 0.0f)
		{
			t = blendTime_ / blendDuration_;
		}

		//tが1.0fを超えないようにする
		if (t > 1.0f) t = 1.0f;

		//新しいアニメーション
		MV1SetAttachAnimBlendRate(modelHandle_, currentAttachAnim_, t);

		//古いアニメーション
		if (prevAttach_ != -1)
		{
			MV1SetAttachAnimBlendRate(modelHandle_, prevAttach_, kBlendRate1 - t);
		}

		//アニメーションのブレンドが完了したら古いアニメをデタッチ
		if (t >= 1.0f)
		{
			if (prevAttach_ != -1)
			{
				MV1DetachAnim(modelHandle_, prevAttach_);
				prevAttach_ = -1;
			}
			isBlending_ = false;
		}
	}
}

void Animation::Play(int animIndex, float speed, bool isLoop)
{
	if (currentAnim_ == animIndex && currentAttachAnim_ != -1) return;

	isAnimEnd_ = false;
	speed_ = speed;
	isLoop_ = isLoop;

	//アニメーションをデタッチ
	if (prevAttach_ != -1)
	{
		MV1DetachAnim(modelHandle_, prevAttach_);
		prevAttach_ = -1;
	}

	//現在再生しているアニメーションを保存
	if (prevState_ == AnimationState::Spawn)
	{
		if (currentAttachAnim_ != -1)
		{
			MV1DetachAnim(modelHandle_, currentAttachAnim_);
		}

		prevAttach_ = -1; //ブレンドなし
	}
	else
	{
		prevAttach_ = currentAttachAnim_;
	}

	currentAnim_ = animIndex;
	currentTime_ = 0.0f;

	currentAttachAnim_ = MV1AttachAnim(modelHandle_, currentAnim_);
	totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttachAnim_);

	blendTime_ = 0.0f;
	isBlending_ = true;

	//ブレンド比率の初期化
	MV1SetAttachAnimBlendRate(modelHandle_, currentAttachAnim_, kBlendRate0);

	if (prevAttach_ != -1)
	{
		//ブレンドを最初はしないまま表示
		MV1SetAttachAnimBlendRate(modelHandle_, prevAttach_, kBlendRate1);
	}

	//アニメーションの動きに応じてブレンド率を適用
	MV1SetAttachAnimTime(modelHandle_, currentAttachAnim_, currentTime_);
}

void Animation::ChangeState(AnimationState state, const std::wstring& animName)
{
	//stateとアニメーション名の名前がどっちも同じ場合だったときのみ何もしない
	if (state_ == state && currentAttachAnim_ != -1 && currentAnimName_ == animName)
	{
		return;
	}
	prevState_ = state_;
	state_ = state;
	currentAnimName_ = animName;

	//animNameで受け取ったアニメーション名を使用する
	int animIndex = MV1GetAnimIndex(modelHandle_, animName.c_str());

	//アニメーションの再生
	if (animIndex != -1)
	{
		bool loop = true;

		float speed = kAnimationSpeed;

		if (state_ == AnimationState::Attack ||
			state_ == AnimationState::JumpAttack ||
			state_ == AnimationState::Guard ||
			state_ == AnimationState::EnemyAttack ||
			state_ == AnimationState::EnemyJumpAttack ||
			state_ == AnimationState::Damage ||
			state_ == AnimationState::KnockBack ||
			state_ == AnimationState::Death ||
			state_ == AnimationState::Spawn)
		{
			loop = false;
		}

		if (state_ == AnimationState::Attack)
		{
			speed = kAnimationSpeed * 1.5f;
		}

		Play(animIndex, speed, loop);
	}
}

void Animation::ChangeState(AnimationState state)
{
	auto it = animNames_.find(state);
	if (it == animNames_.end())
	{
		return;
	}

	//登録したアニメーション名を使用してもともとあるChangeStateを呼ぶ
	ChangeState(state, it->second);
}

void Animation::RegisterAnimName(AnimationState state, const std::wstring& animName)
{
	animNames_[state] = animName;
}

void Animation::ResetAnimation()
{
	currentTime_ = 0.0f;
	isAnimEnd_ = false;
}

void Animation::SetSlowAnimationSpeed()
{
	speed_ = kSlowAnimationSpeed;
}

void Animation::SetEnemyAttackAnimationSpeed()
{
	speed_ = kEnemyAttackAnimationSpeed;
}
