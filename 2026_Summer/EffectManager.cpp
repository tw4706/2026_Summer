#include "EffectManager.h"
#include"EffekseerForDXLib.h"
#include "Application.h"
#include<cassert>

EffectManager& EffectManager::GetInstance()
{
	static EffectManager instance;
	return instance;
}

void EffectManager::Update()
{
	for (auto it = handles_.begin(); it != handles_.end();)
	{
		if (IsEffekseer3DEffectPlaying(*it) == false)
		{
			it = handles_.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void EffectManager::Load(const std::wstring& name, const std::wstring& path)
{
	//すでにロードされていたら何もしない
	if (effects_.count(name) > 0)
	{
		return;
	}

	int handle = LoadEffekseerEffect(path.c_str());
	assert(handle >= 0 && "エフェクトのロードに失敗しました");
	effects_[name] = handle;
}

int EffectManager::Play(const std::wstring& name, const Vector3& pos)
{
	assert(effects_.count(name) > 0);

	int handle = PlayEffekseer3DEffect(effects_[name]);

	SetPosPlayingEffekseer3DEffect(
		handle,
		pos.x_, pos.y_, pos.z_);

	handles_.push_back(handle);

	return handle;
}

void EffectManager::Stop(int handle)
{
	StopEffekseer3DEffect(handle);

	//エフェクトを停止したらリストからも削除する
	handles_.erase(std::remove(handles_.begin(), handles_.end(), handle),handles_.end());
}

void EffectManager::StopAll()
{
	//エフェクトの停止
	for (auto& handle : handles_)
	{
		StopEffekseer3DEffect(handle);
	}

	handles_.clear();
}

void EffectManager::Clear()
{
	//すべてのエフェクトの再生を止める
	StopAll();

	//エフェクトの削除
	for (auto& effct : effects_)
	{
		DeleteEffekseerEffect(effct.second);
	}

	//エフェクトのクリア
	effects_.clear();
}
