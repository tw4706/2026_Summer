#pragma once
#include "UIBase.h"
class RethicleUI :public UIBase
{
public:
	RethicleUI();
	virtual~RethicleUI();

	virtual void Init()override;
	virtual void Update()override;
	virtual void Draw()override;
};

