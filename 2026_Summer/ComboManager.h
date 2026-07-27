#pragma once
#include"ComboDataLoader.h"
#include<string>

class ComboManager
{
public:
	ComboManager();
	virtual ~ComboManager()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	bool Init(const std::wstring& filePath);

	/// <summary>
	/// 攻撃の入力が行われた時の関数
	/// </summary>
	/// <param name="currentFrame">現在のフレーム</param>
	void OnAttackInput(float currentFrame);

	/// <summary>
	/// 入力の受付が可能かどうか
	/// </summary>
	/// <param name="currentFrame">現在のフレーム</param>
	/// <returns>入力が可能な場合はtrue,入力ができない場合はfalse</returns>
	bool CanAcceptInput(float currentFrame)const;

	/// <summary>
	/// 次のコンボに遷移可能かどうか
	/// </summary>
	/// <param name="currentFrame">現在のフレーム</param>
	/// <returns>遷移可能だったらtrue,そうでない場合はfalse</returns>
	bool CanTransToNextCombo(float currentFrame)const;

	/// <summary>
	/// 次のコンボに進める
	/// </summary>
	void AdvancedCombo();

	/// <summary>
	/// コンボのリセットを行う
	/// </summary>
	void ResetCombo();

	/// <summary>
	/// 現在の段数のコンボデータを取得する
	/// </summary>
	/// <returns>見つかればそのデータへのポインタを見つからなければnullptrを返す</returns>
	const ComboData* GetCurrentComboData() const;

	/// <summary>
	/// 現在のコンボの段を取得する
	/// </summary>
	int GetCurrentComboCount() const { return comboCount_; }

	/// <summary>
	/// 次のコンボのフラグが立っているか
	/// </summary>
	bool IsNextComboRequested() const { return isNextComboRequested_; }

	/// <summary>
	/// 次のコンボのフラグをクリアする
	/// </summary>
	void ClearNextComboRequest() { isNextComboRequested_ = false; }

	/// <summary>
	/// 現在最終段かどうか
	/// </summary>
	bool IsMaxCombo() const;

private:
	/// <summary>
	/// 段数からCSV上のキー文字列を組み立てる（例：1段目なら"Player|Combo1"）
	/// </summary>
	std::string MakeComboKey(int comboCount) const;

private:
	ComboDataLoader loader_;			//コンボデータのローダー

	int comboCount_;					//コンボのカウンタ
	int maxCombo_;						//最大コンボ数
	bool isNextComboRequested_ = false;	//次のコンボのリクエストするフラグ

};

