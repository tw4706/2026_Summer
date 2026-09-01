#include "SceneManager.h"
#include "Scene.h"

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance;
	return instance;
}

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::ResetScene(std::shared_ptr<Scene> scene)
{
	scenes_.clear();
	scenes_.push_back(scene);
	scene->Init();
}

void SceneManager::ChangeScene(std::shared_ptr<Scene> scene)
{
	//ƒV[ƒ“‚ª‹ó‚Ìê‡‚Í
	if (scenes_.empty())
	{
		//­‚È‚­‚Æ‚à1‚Â‚ÍÏ‚Ü‚ê‚Ä‚¢‚éó‘Ô‚É‚·‚é
		scenes_.push_back(scene);
	}
	else
	{
		scenes_.back() = scene;
	}
	scene->Init();
}

void SceneManager::PushScene(std::shared_ptr<Scene> scene)
{
	scenes_.push_back(scene);
	scene->Init();
}

void SceneManager::PopScene()
{
	if (!scenes_.empty())
	{
		scenes_.pop_back();
	}
}

void SceneManager::RemoveScene()
{
	if (scenes_.size() >= 2)
	{
		auto it = std::prev(scenes_.end(), 2);
		scenes_.erase(it);
	}
}

void SceneManager::Update()
{
	//––”ö‚Ì—v‘f‚Ì‚İUpdate‚·‚é
	scenes_.back()->Update();
}
void SceneManager::Draw()
{
	for (auto& scene : scenes_)
	{
		scene->Draw();
	}
}