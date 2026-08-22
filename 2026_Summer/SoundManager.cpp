#include "SoundManager.h"
#include<Dxlib.h>

namespace
{
	constexpr int kBaseBgmVolume = 160;
	constexpr int kBaseSeVolume = 220;

	//各BGM・SEのパス
	const wchar_t* kTitleBgm = L"data/BGM・SE/TitleBGM.mp3";
	const wchar_t* kGameBgm = L"data/BGM・SE/GameBGM.mp3";
	const wchar_t* kBossBgm = L"data/BGM・SE/BossBattleBGM.mp3";
	const wchar_t* kResultBgm = L"data/BGM・SE/ResultBGM.mp3";

	const wchar_t* kDecideSe = L"data/BGM・SE/Decide.mp3";
	const wchar_t* kAttackSe = L"data/BGM・SE/Attack.mp3";
	const wchar_t* kMoveSe = L"data/BGM・SE/Move.mp3";
	const wchar_t* kPlayerHitSe = L"data/BGM・SE/PlayerHit.mp3";
	const wchar_t* kEnemyHitSe = L"data/BGM・SE/EnemyHit.mp3";
	const wchar_t* kDodgeSe = L"data/BGM・SE/Dodge.mp3";
	const wchar_t* kDeathSe = L"data/BGM・SE/Death.mp3";
	const wchar_t* kGuardSe = L"data/BGM・SE/Guard.mp3";
	const wchar_t* kJumpSe = L"data/BGM・SE/Jump.mp3";
	const wchar_t* kLockOnSe = L"data/BGM・SE/LockOn.mp3";
	const wchar_t* kCursoleMoveSe = L"data/BGM・SE/MoveCursole.mp3";
}

SoundManager& SoundManager::GetInstance()
{
	static SoundManager instance;
	return instance;
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
	//音量の設定
	seVolume_ = kBaseSeVolume;
	bgmVolume_ = kBaseBgmVolume;

	//各BGM・SEの読み込み
	bgmHandles_[BGM::Title] = LoadSoundMem(kTitleBgm);
	bgmHandles_[BGM::Game] = LoadSoundMem(kGameBgm);
	bgmHandles_[BGM::BossBattle] = LoadSoundMem(kBossBgm);
	bgmHandles_[BGM::Result] = LoadSoundMem(kResultBgm);

	seHandles_[SE::Decide] = LoadSoundMem(kDecideSe);
	seHandles_[SE::Attack] = LoadSoundMem(kAttackSe);
	seHandles_[SE::PlayerHit] = LoadSoundMem(kPlayerHitSe);
	seHandles_[SE::EnemyHit] = LoadSoundMem(kEnemyHitSe);
	seHandles_[SE::Dodge] = LoadSoundMem(kDodgeSe);
	seHandles_[SE::Death] = LoadSoundMem(kDeathSe);
	seHandles_[SE::Guard] = LoadSoundMem(kGuardSe);
	seHandles_[SE::Jump] = LoadSoundMem(kJumpSe);
	seHandles_[SE::LockOn] = LoadSoundMem(kLockOnSe);
	seHandles_[SE::CursoleMove] = LoadSoundMem(kCursoleMoveSe);
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
