#include "SoundManager.h"
#include<Dxlib.h>

namespace
{
	constexpr int kBaseBgmVolume = 160;
	constexpr int kBaseSeVolume = 220;

	//各BGM・SEのパス
	const wchar_t* kTitleBgm = L"data/BGM・SE/Title.mp3";
	const wchar_t* kGameBgm = L"data/BGM・SE/Game.mp3";
	const wchar_t* kResultBgm = L"data/BGM・SE/Result.mp3";

	const wchar_t* kDecideSe = L"data/BGM・SE/decide.mp3";
	const wchar_t* kAttackSe = L"data/BGM・SE/attack.mp3";
	const wchar_t* kMoveSe = L"data/BGM・SE/move.mp3";
	const wchar_t* kHitSe = L"data/BGM・SE/enemyHit.mp3";
	const wchar_t* kDodgeSe = L"data/BGM・SE/dodge.mp3";
	const wchar_t* kJustDodgeSe = L"data/BGM・SE/justDodge.wav";
	const wchar_t* kPlayerDeathSe = L"data/BGM・SE/playerDeath.mp3";
	const wchar_t* kCountDownSe = L"data/BGM・SE/ready.mp3";
	const wchar_t* kStartSe = L"data/BGM・SE/go.mp3";
	const wchar_t* kCancelSe = L"data/BGM・SE/cancel.mp3";
	const wchar_t* kAreaLockSe = L"data/BGM・SE/areaLock.mp3";
	const wchar_t* kAreaReleaseSe = L"data/BGM・SE/areaRelease.mp3";
}

SoundManager::SoundManager():
	currentSeHandle_(-1),
	currentBgmHandle_(-1)
{
}

SoundManager::~SoundManager()
{
	//BGMハンドルの削除
	for (auto bgm : bgmHandles_)
	{
		DeleteSoundMem(bgm.second);
	}

	//SEハンドルの削除
	for (auto se : seHandles_)
	{
		DeleteSoundMem(se.second);
	}
}

void SoundManager::Init()
{
	seVolume_ = kBaseSeVolume;
	bgmVolume_ = kBaseBgmVolume;

	bgmHandles_[BGM::Title] = LoadSoundMem(kTitleBgm);
	bgmHandles_[BGM::Game] = LoadSoundMem(kGameBgm);
	bgmHandles_[BGM::Result] = LoadSoundMem(kResultBgm);

	seHandles_[SE::Decide] = LoadSoundMem(kDecideSe);
	seHandles_[SE::Attack] = LoadSoundMem(kAttackSe);
	seHandles_[SE::Move] = LoadSoundMem(kMoveSe);
	seHandles_[SE::Hit] = LoadSoundMem(kHitSe);
	seHandles_[SE::Dodge] = LoadSoundMem(kDodgeSe);
	seHandles_[SE::JustDodge] = LoadSoundMem(kJustDodgeSe);
	seHandles_[SE::PlayerDeath] = LoadSoundMem(kPlayerDeathSe);
	seHandles_[SE::CountDown] = LoadSoundMem(kCountDownSe);
	seHandles_[SE::Start] = LoadSoundMem(kStartSe);
	seHandles_[SE::Cancel] = LoadSoundMem(kCancelSe);
	seHandles_[SE::AreaLock] = LoadSoundMem(kAreaLockSe);
	seHandles_[SE::AreaRelease] = LoadSoundMem(kAreaReleaseSe);
}

void SoundManager::PlaySe(SE se)
{
	int handle = seHandles_[se];

	//SEの音量
	ChangeVolumeSoundMem(seVolume_, handle);

	//SEの再生	
	PlaySoundMem(handle, DX_PLAYTYPE_BACK,TRUE);
}

void SoundManager::PlayBgm(BGM bgm)
{
	int handle = bgmHandles_[bgm];

	if (currentBgmHandle_ == handle) return;

	if (currentBgmHandle_ != -1)
	{
		StopSoundMem(currentBgmHandle_);
	}

	//再生しているBGMハンドルを更新する
	currentBgmHandle_ = handle;

	//BGMの音量
	ChangeVolumeSoundMem(bgmVolume_,currentBgmHandle_);

	//BGM再生
	PlaySoundMem(currentBgmHandle_, DX_PLAYTYPE_LOOP);
}
