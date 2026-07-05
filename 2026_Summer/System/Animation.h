#pragma once
#include<string>
#include<unordered_map>

enum class AnimationState
{
	None,
	Spawn,
	Idle,
	Walk,
	Run,
	Damage,
	Attack,
	EnemyAttack,
	PrevAttack,
	Death,
	Dodge,
	Jump,
};

class Animation
{
public:

	Animation();
	~Animation();

	void Init(int modelHandle);
	void Update(float deltaTime);

	//モデルのアニメーションの再生
	void Play(int animIndex, float speed, bool isLoop);

	/// <summary>
	/// アニメーションの現在の時間を取得する
	/// </summary>
	/// <returns>アニメーションの現在時間</returns>
	float GetCurrentAnimTime()const { return currentTime_; }

	/// <summary>
	/// アニメーションの状態を変更します。
	/// </summary>
	/// <param name="state">アニメーションの状態</param>
	/// <param name="animName">変更するアニメーション名</param>
	void ChangeState(AnimationState state, const std::wstring& animName);

	/// <summary>
	/// アニメーションの状態を変更
	/// </summary>
	/// <param name="state">アニメーションの状態</param>
	//上のChangeStateと違う点はこっちはCSVから読み込んだステータスを入れる
	void ChangeState(AnimationState state);

	/// <summary>
	/// 状態とアニメーション名を紐づけて登録する
	/// </summary>
	/// <param name="state">アニメーションの状態</param>
	/// <param name="animName">対応するアニメーション名</param>
	void RegisterAnimName(AnimationState state, const std::wstring& animName);

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

	/// <summary>
	/// アニメーションのリセット
	/// </summary>
	void ResetAnimation();

	/// <summary>
	/// スローのアニメーション速度を設定
	/// </summary>
	void SetSlowAnimationSpeed();

	/// <summary>
	/// 敵の攻撃アニメーションの速度設定
	/// </summary>
	void SetEnemyAttackAnimationSpeed();

private:
	int modelHandle_;           //モデルハンドル

	//現在アニメ
	int currentAttachAnim_;     //アタッチされるアニメーション
	int currentAnim_;           //現在のアニメーション
	float currentTime_;         //現在の時間
	int prevAttach_;            //前のアニメ

	float blendTime_;           //経過時間
	float blendDuration_;       //ブレンド時間
	bool isBlending_;           //ブレンドされているか
	float speed_;               //スピード
	bool isLoop_;               //ループするかどうか
	bool isAnimEnd_;            //アニメーションが終わったかどうか
	float totalTime_;           //アニメーションのトータルタイム

	AnimationState state_;      //アニメーションの状態
	AnimationState prevState_;  //アニメーションの前の状態
	std::unordered_map<AnimationState, std::wstring>animNames_;//状態ごとのアニメーション名
};

