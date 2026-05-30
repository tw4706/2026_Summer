#pragma once

enum class AnimType
{
    Player,
    Enemy
};

enum class AnimationState
{
    Spawn,
    Idle,
    Run,
    Hit,
    Attack,
    PrevAttack,
    Death,
    Dodge
};

class Animation
{
public:
	Animation();
	~Animation();

	void Init(int modelHandle,AnimType type);
    void Update(float deltaTime);

	//モデルのアニメーションの再生
	void Play(int animIndex, float speed, bool isLoop);

    /// <summary>
    /// アニメーションの状態遷移
    /// </summary>
    /// <param name="state">状態名</param>
    void ChangeState(AnimationState state);

    /// <summary>
    /// アニメーション状態の取得
    /// </summary>
    /// <returns>アニメーション状態</returns>
    AnimationState GetState()const { return state_; }

    /// <summary>
    /// アニメーションの終了を通知する関数
    /// </summary>
    /// <returns>アニメーションが終了したかどうか</returns>
    bool IsEnd()const { return isAnimEnd_; }


private:
	int modelHandle_;

    //現在アニメ
    int currentAttach_;
    int currentAnim_;
    float currentTime_;

    int prevAttach_;      // 前のアニメ
    float blendTime_;     // 経過時間
    float blendDuration_; // ブレンド時間
    bool isBlending_;
    float speed_;
    bool isLoop_;
    bool isAnimEnd_;
    float totalTime_;

    AnimationState state_;
    AnimationState prevState_;
    AnimType type_;
};

