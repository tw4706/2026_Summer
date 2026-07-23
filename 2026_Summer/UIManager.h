#pragma once
#include<vector>
#include<memory>

class UIBase;
class UIManager
{
public:
	UIManager();
	virtual~UIManager();

	void Init();
	void Update();
	void Draw();

	//UI‚Ì“o˜^
	void AddUI(const std::shared_ptr<UIBase>& pUI);

	//UI‚Ìíœ
	void RemoveUI(const std::shared_ptr<UIBase>& pUI);

	//‘SUI‚Ìíœ
	void Clear();

private:
	std::vector<std::shared_ptr<UIBase>>pUILists_;//UI‚Ì”z—ñ
};

