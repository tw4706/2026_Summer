#include "Animation.h"
#include<Dxlib.h>
#include <cmath>

namespace
{
	//プレイヤーアニメーション
	//Idle
	const char* kPlayerIdle = "Player|Idle";
	//Run
	const char* kPlayerRun = "Player|Run";
	//Attack
	const char* kPlayerAttack = "Player|Attack";
	//Hit
	const char* kPlayerHit = "Player|Hit";
	//Death
	const char* kPlayerDeath = "Player|Death";

	//敵アニメーション
	//Spawn
	const char* kEnemySpawn = "MonsterArmature|Jump";
	//Idle
	const char* kEnemyIdle = "MonsterArmature|Idle";
	//Run
	const char* kEnemyRun = "MonsterArmature|Dance";
	//Attack
	const char* kEnemyAttack = "MonsterArmature|Bite_Front";
	//PrevAttack
	const char* kEnemyPrevAttack = "MonsterArmature|No";
	//Death
	const char* kEnemyDeath = "MonsterArmature|Death";

	//アニメーションの進むスピード
	constexpr float kAnimationSpeed = 30.0f;
	//攻撃アニメーションの進むスピード
	constexpr float kAttackAnimationSpeed = 60.0f;
}

Animation::Animation() :
	modelHandle_(-1),
	currentAttach_(-1),
	currentAnim_(-1),
	currentTime_(0.0f),
	prevAttach_(-1),
	blendTime_(0.0f),
	blendDuration_(0.3f),
	isBlending_(false),
	speed_(1.0f),
	isLoop_(true),
	isAnimEnd_(false),
	totalTime_(0.0f),
	state_(AnimationState::Idle),
	prevState_(AnimationState::Idle),
	type_(AnimType::Player)
{

}

Animation::~Animation()
{
}

void Animation::Init(int modelHandle,AnimType type)
{
	modelHandle_ = modelHandle;
	type_ = type;
}

void Animation::Update(float deltaTime)
{
	if (currentAttach_ != -1 && totalTime_ == 0.0f)
	{
		totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttach_);
	}

	//アニメーションの更新
	if (currentAttach_ != -1)
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
		MV1SetAttachAnimTime(modelHandle_, currentAttach_, currentTime_);

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

		float t = 1.0f;

		//ブレンドタイムがDurationを超えないように0.0f~1.0fで正規化する
		if (blendDuration_ > 0.0f)
		{
			t = blendTime_ / blendDuration_;
		}

		//tが1.0fを超えないようにする
		if (t > 1.0f) t = 1.0f;

		//新アニメ
		MV1SetAttachAnimBlendRate(modelHandle_, currentAttach_, t);

		//旧アニメ
		if (prevAttach_ != -1)
		{
			MV1SetAttachAnimBlendRate(modelHandle_, prevAttach_, 1.0f - t);
		}

		//アニメーションのブレンドが完了したら旧アニメをデタッチする
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

	if (currentAnim_ == animIndex && currentAttach_ != -1) return;

	isAnimEnd_ = false;
	speed_ = speed;
	isLoop_ = isLoop;

	//アニメーションをデタッチ
	if (prevAttach_ != -1)
	{
		MV1DetachAnim(modelHandle_, prevAttach_);
		prevAttach_ = -1;
	}

	//現在再生しているアニメーションを保存(生成状態の時はブレンドさせない)
	if (prevState_ == AnimationState::Spawn)
	{
		if (currentAttach_ != -1)
		{
			MV1DetachAnim(modelHandle_, currentAttach_);
		}

		prevAttach_ = -1; //ブレンドなし
	}
	else
	{
		prevAttach_ = currentAttach_;
	}

	currentAnim_ = animIndex;
	currentTime_ = 0.0f;

	currentAttach_ = MV1AttachAnim(modelHandle_, currentAnim_);
	totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttach_);

	blendTime_ = 0.0f;
	isBlending_ = true;

	//ブレンドの比率の初期化
	MV1SetAttachAnimBlendRate(modelHandle_, currentAttach_, 0.0f);

	if (prevAttach_ != -1)
	{
		//ブレンドを最初はしないまま表示
		MV1SetAttachAnimBlendRate(modelHandle_, prevAttach_, 1.0f);
	}

	//アニメーションの動きに応じてブレンド率を適用
	MV1SetAttachAnimTime(modelHandle_, currentAttach_, currentTime_);
}

void Animation::ChangeState(AnimationState state)
{
	if (state_ == state && currentAttach_ != -1) return;

	prevState_=state_;
	state_ = state;

	int animIndex = -1;

	//状態ごとのアニメーションインデックスの取得
	if (type_ == AnimType::Player)
	{
		switch (state_)
		{
		case AnimationState::Idle:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerIdle);
			break;
		case AnimationState::Run:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerRun);
			break;
		case AnimationState::Attack:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerAttack);
			break;
		case AnimationState::Hit:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerHit);
			break;
		case AnimationState::Death:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerDeath);
			break;
		}
	}
	//敵も同様
	else if (type_ == AnimType::Enemy)
	{
		switch (state_)
		{
		case AnimationState::Idle:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyIdle);
			break;
		case AnimationState::Run:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyRun);
			break;
		case AnimationState::Attack:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyAttack);
			break;
		case AnimationState::PrevAttack:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyPrevAttack);
			break;
		case AnimationState::Death:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyDeath);
			break;
		case AnimationState::Spawn:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemySpawn);
			break;
		}
	}

	//アニメーションの再生
	if (animIndex != -1)
	{
		bool loop = true;

		float speed = kAnimationSpeed;

		if (state_ == AnimationState::Attack ||
			state_ == AnimationState::Death||
			state_ == AnimationState::Spawn)
		{
			loop = false;
		}

		if (state_ == AnimationState::Attack)
		{
			speed = kAttackAnimationSpeed;
		}

		Play(animIndex, speed, loop);
	}
}
