#pragma once
#include<map>

enum class BGM
{
	Title,
	Game,
	BossBattle,
	Result
};
enum class BGMState
{
	None,
	Normal,
	FadeIn,
	FadeOut
};

enum class SE
{
	Decide,
	Attack,
	Move,
	PlayerHit,
	EnemyHit,
	Dodge,
	Death,
	Guard,
	Jump,
	LockOn,
	CursoleMove,
	BossSlash,
	BossAttack
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
	/// 更新処理
	/// </summary>
	void Update();

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
	//SEとBGMのハンドルを保有しておくマップ
	std::map<BGM, int>bgmHandles_;
	std::map<SE, int>seHandles_;

	//現在のSEのハンドル
	int currentSeHandle_ = -1;
	//現在のBGMのハンドル
	int currentBgmHandle_ = -1;
	//SEの音量
	int seVolume_ = 0;
	//BGMの音量
	int bgmVolume_ = 0;

	//BGMのステート
	BGMState state_ = BGMState::None;

	//次に再生するBGM
	BGM nextBgmState_ = BGM::Title;

	//BGMのフェードイン・フェードアウトの音量
	float fadeBGMVolume_ = 0.0f;
};

