#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Model.h"
#include <memory>

class CharacterStateBase;
class Character : public GameObject, public std::enable_shared_from_this<Character>
{
public:
	Character(Vector3 pos, Vector3 vel, float dir);
	virtual ~Character();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;

	/// <summary>
	/// 状態の切り替え
	/// </summary>
	void ChangeState(std::shared_ptr<CharacterStateBase> pNextState);

	/// <summary>
	/// アニメーションの遷移
	/// </summary>
	/// <param name="state">アニメーションの状態</param>
	void ChangeAnimation(AnimationState state, const std::wstring& animName);

	/// <summary>
	/// アニメーションが終了したかどうかを判定します。
	/// </summary>
	/// <returns>アニメーションが終了している場合はtrueを、そうでない場合はfalseを返す。</returns>
	bool IsAnimationEnd() const { return animation_.IsEnd(); }

	bool GetIsGround() const { return isGround_; }
	void SetIsGround(bool isGround) { isGround_ = isGround; }

	float GetGravity() const { return gravity_; }

protected:
	int hp_;
	bool isGround_ = true;					//地面についているかどうか
	float gravity_ = 0.0f;					//重力の強さ

	Model model_;                           //モデル
	Animation animation_;                   //アニメーション
	std::shared_ptr<CharacterStateBase> pCurrentState_ = nullptr;//現在のステートを管理するポインタ
};

