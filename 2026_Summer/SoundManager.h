#pragma once
#include<map>

enum class BGM
{
	Title,
	Game,
	Result
};

enum class SE
{
	Decide,
	Attack,
	Move,
	Hit,
	Dodge,
	PlayerDeath,
	Start,
	Cancel,
	CursoleMove
};

class SoundManager
{
public:
	static SoundManager& GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// SEの再生
	/// </summary>
	/// <param name="se">SEタイプ</param>
	void PlaySe(SE se);

	/// <summary>
	/// BGMの再生
	/// </summary>
	/// <param name="bgm">BGMタイプ</param>
	void PlayBgm(BGM bgm);
private:
	SoundManager();
	~SoundManager();

	SoundManager(const SoundManager&) = delete;
	void operator=(const SoundManager&) = delete;

private:
	std::map<BGM, int>bgmHandles_;
	std::map<SE, int>seHandles_;

	int currentSeHandle_ = -1;
	int currentBgmHandle_ = -1;
	int seVolume_ = 0;
	int bgmVolume_ = 0;
};

