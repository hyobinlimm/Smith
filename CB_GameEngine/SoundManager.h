#pragma once

#include "fmod.hpp"
#include "fmod_errors.h"

using namespace FMOD;

class SoundDataTable;

class SoundManager
{
public:
	SoundManager() = default;

	~SoundManager() = default;

	SoundManager(const SoundManager& other) = delete;

	SoundManager& operator= (const SoundManager& other) = delete;

	SoundManager(SoundManager&& other) = delete;

public:
	static shared_ptr<SoundManager> GetInstance();

	void Initialize(string FilePath);

	void Update();
	void BgmSetting(string soundName);
	void BgmPlay();
	void BgmPause();
	void BgmSoundControl(float volume);

	void SFXPlay(string soundName, int channel);
	void CharPlay(string soundName);
	void SFXPause(string soundName, int channel);

	void BgmChange(int num);

private:
	static shared_ptr<SoundManager> g_SoundInstance;

	shared_ptr<SoundDataTable> m_SoundData;

	System* m_System;

	map<string, Sound*> m_MainMenuBgm;

	map<string, Sound*> m_GameSceneBgm;
	map<string, Sound*> m_GameSceneSFX;

	map<string, Sound*> m_BadEndingBgm;
	map<string, Sound*> m_NormalEndingBgm;
	map<string, Sound*> m_HappyEndingBgm;

	Channel* m_BgmChannel;
	Channel* m_CharChannel;
	Channel* m_SFXChannel[40];
};