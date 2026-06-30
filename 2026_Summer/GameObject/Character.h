#pragma once
#include "Collider/Collidable.h"
#include "Animation.h"
#include "Model.h"
#include <memory>

class CharacterStateBase;
class Character : public Collidable, public std::enable_shared_from_this<Character>
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
	/// アニメーションの遷移(オーバーロード)
	/// </summary>
	/// <param name="state">アニメーション名</param>
	void ChangeAnimation(AnimationState state);

	/// <summary>
	/// アニメーションが終了したかどうかを判定します。
	/// </summary>
	/// <returns>アニメーションが終了している場合はtrueを、そうでない場合はfalseを返す。</returns>
	bool IsAnimationEnd() const { return animation_.IsEnd(); }

	/// <summary>
	/// 重力の取得
	/// </summary>
	/// <returns>重力を返す</returns>
	float GetGravity() const { return gravity_; }

protected:
	int hp_;
	float gravity_ = 0.0f;					//重力の強さ

	Model model_;                           //モデル
	Animation animation_;                   //アニメーション
	std::shared_ptr<CharacterStateBase> pCurrentState_ = nullptr;//現在のステートを管理するポインタ
};

