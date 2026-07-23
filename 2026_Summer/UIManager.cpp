#include "UIManager.h"
#include "../UI/UIBase.h"

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

void UIManager::Init()
{
	//‚·‚×‚Ä‚ÌUI‚Ì‰Šú‰»
	for (auto& ui : pUILists_)
	{
		ui->Init();
	}
}
void UIManager::Update()
{
	//‚·‚×‚Ä‚ÌUI‚ÌXV
	for (auto& ui : pUILists_)
	{
		ui->Update();
	}
}
void UIManager::Draw()
{
	//‚·‚×‚Ä‚ÌUI‚Ì•`‰æ
	for (auto& ui : pUILists_)
	{
		ui->Draw();
	}
}

void UIManager::AddUI(const std::shared_ptr<UIBase>& pUI)
{
	if (!pUI)return;

	pUILists_.push_back(pUI);

	//UIManager‚Ì‰Šú‰»‚ðs‚¤
	pUI->Init();
}

void UIManager::RemoveUI(const std::shared_ptr<UIBase>& pUI)
{
	auto it = std::remove(pUILists_.begin(), pUILists_.end(), pUI);
	pUILists_.erase(it, pUILists_.end());
}

void UIManager::Clear()
{
	pUILists_.clear();
}
