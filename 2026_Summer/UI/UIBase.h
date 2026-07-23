#pragma once
class UIBase
{
public:
	UIBase();
	virtual~UIBase();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;
};

