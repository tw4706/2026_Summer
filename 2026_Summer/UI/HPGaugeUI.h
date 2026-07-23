#pragma once
#include "UIBase.h"
class HPGaugeUI :public UIBase
{
public:
	HPGaugeUI();
	virtual~HPGaugeUI();

	virtual void Init()override;
	virtual void Update()override;
	virtual void Draw()override;

protected:

	/// <summary>
	/// HPゲージのUIのパスを取得する
	/// </summary>
	/// <returns>HPUIのパスを返す</returns>
	virtual const wchar_t* GetHPGraphPath()const = 0;

	/// <summary>
	/// HPの更新
	/// </summary>
	virtual void UpdateHP() = 0;

	/// <summary>
	/// 描画座標に表示するかしないか
	/// </summary>
	virtual bool VisibleDrawPos(int& drawX,int&drawY)const = 0;

protected:
	int hpUIHandle_;						//HPUIハンドル
	int hpUIFrameHandle_;					//HPバーUIハンドル
	int hpUIX_, hpUIY_;						//グラフィックハンドルのサイズ取得用
	int hpBarUIX_, hpBarUIY_;				//グラフィックハンドルのサイズ取得用

	float currentHP_;						//現在の体力
	float maxHP_;							//最大体力

	float scale_;							//拡大率
};

