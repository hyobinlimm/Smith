#include "CBpch.h"
#include "SoundManager.h"
#include "SoundDataTable.h"
#include "InputManager.h"

shared_ptr<SoundManager> SoundManager::g_SoundInstance = nullptr;

std::shared_ptr<SoundManager> SoundManager::GetInstance()
{
	if (g_SoundInstance == nullptr)
		g_SoundInstance = make_shared<SoundManager>();

	return g_SoundInstance;
}

void SoundManager::Initialize(string FilePath)
{
	System_Create(&m_System);

	m_System->init(10, FMOD_INIT_NORMAL, NULL);

	m_SoundData = make_shared<SoundDataTable>();

	m_SoundData->DeserializeFromFile(FilePath);

	for (auto& soundData : m_SoundData->m_SoundList)
	{
		Sound* _NewSound;
		if (soundData->m_Scene == "#1: Main Menu")
		{
			m_System->createSound(("../Resource/Sound/" + soundData->m_FileName + ".mp3").c_str(), FMOD_LOOP_NORMAL | FMOD_DEFAULT, NULL, &_NewSound);
			m_MainMenuBgm.insert({ soundData->m_Name, _NewSound });
		}
		else if (soundData->m_Scene == "#2: Game Scene")
		{
			if (soundData->m_Loop == "TRUE")
			{
				m_System->createSound(("../Resource/Sound/" + soundData->m_FileName + ".mp3").c_str(), FMOD_LOOP_NORMAL | FMOD_DEFAULT, NULL, &_NewSound);
				m_GameSceneBgm.insert({ soundData->m_Name, _NewSound });
			}
			else if (soundData->m_Loop == "FALSE")
			{
				m_System->createSound(("../Resource/Sound/" + soundData->m_FileName + ".mp3").c_str(), FMOD_DEFAULT, NULL, &_NewSound);
				m_GameSceneSFX.insert({ soundData->m_Name, _NewSound });
			}
		}
		else if (soundData->m_Scene == "#3: Bad Ending Scene")
		{
			m_System->createSound(("../Resource/Sound/" + soundData->m_FileName + ".mp3").c_str(), FMOD_LOOP_NORMAL | FMOD_DEFAULT, NULL, &_NewSound);
			m_BadEndingBgm.insert({ soundData->m_Name, _NewSound });
		}
		else if (soundData->m_Scene == "#4: Normal Ending Scene")
		{
			m_System->createSound(("../Resource/Sound/" + soundData->m_FileName + ".mp3").c_str(), FMOD_LOOP_NORMAL | FMOD_DEFAULT, NULL, &_NewSound);
			m_NormalEndingBgm.insert({ soundData->m_Name, _NewSound });
		}
		else if (soundData->m_Scene == "#5: Happy Ending Scene")
		{
			m_System->createSound(("../Resource/Sound/" + soundData->m_FileName + ".mp3").c_str(), FMOD_LOOP_NORMAL | FMOD_DEFAULT, NULL, &_NewSound);
			m_HappyEndingBgm.insert({ soundData->m_Name, _NewSound });
		}
	}
}

void SoundManager::BgmSetting(string soundName)
{
	for (auto iter = m_MainMenuBgm.begin(); iter != m_MainMenuBgm.end(); iter++)
	{
		if (iter->first == soundName)
		{
			m_BgmChannel->stop();
			m_System->playSound(iter->second, NULL, 0, &m_BgmChannel);
			return;
		}
	}
	for (auto iter = m_GameSceneBgm.begin(); iter != m_GameSceneBgm.end(); iter++)
	{
		if (iter->first == soundName)
		{
			m_BgmChannel->stop();
			m_System->playSound(iter->second, NULL, 0, &m_BgmChannel);
			return;
		}
	}
	for (auto iter = m_BadEndingBgm.begin(); iter != m_BadEndingBgm.end(); iter++)
	{
		if (iter->first == soundName)
		{
			m_BgmChannel->stop();
			m_System->playSound(iter->second, NULL, 0, &m_BgmChannel);
			return;
		}
	}
	for (auto iter = m_NormalEndingBgm.begin(); iter != m_NormalEndingBgm.end(); iter++)
	{
		if (iter->first == soundName)
		{
			m_BgmChannel->stop();
			m_System->playSound(iter->second, NULL, 0, &m_BgmChannel);
			return;
		}
	}
	for (auto iter = m_HappyEndingBgm.begin(); iter != m_HappyEndingBgm.end(); iter++)
	{
		if (iter->first == soundName)
		{
			m_BgmChannel->stop();
			m_System->playSound(iter->second, NULL, 0, &m_BgmChannel);
		}
	}
}

void SoundManager::BgmPlay()
{
	m_BgmChannel->setPaused(false);
}

void SoundManager::BgmPause()
{
	/*for (int i = 0; i < m_SoundData->m_DataCount; i++)
	{
		if (soundName == m_SoundData->m_Name[i])
			m_BgmChannel[i]->setPaused(true);
	}*/
}

void SoundManager::BgmSoundControl(float volume)
{
	m_BgmChannel->setVolume(volume);
}

void SoundManager::SFXPlay(string soundName, int channel)
{
	for (auto iter = m_GameSceneSFX.begin(); iter != m_GameSceneSFX.end(); iter++)
	{
		// 임시
		bool _isPlaying;
		m_SFXChannel[channel]->isPlaying(&_isPlaying);
		if (soundName == "Minigame_Casting" || soundName == "Minigame_Melting" || soundName == "Minigame_Countdown")
		{
			if (iter->first == soundName && !_isPlaying)
			{
				m_System->playSound(iter->second, NULL, 0, &m_SFXChannel[channel]);
				return;
			}
		}
		else
		{
			if (iter->first == soundName)
			{
				m_System->playSound(iter->second, NULL, 0, &m_SFXChannel[channel]);
				return;
			}
		}
	}
}

void SoundManager::CharPlay(string soundName)
{
	for (auto iter = m_GameSceneSFX.begin(); iter != m_GameSceneSFX.end(); iter++)
	{
		// 임시
		bool _isPlaying;
		m_CharChannel->isPlaying(&_isPlaying);
		if (iter->first == soundName && !_isPlaying)
		{
			m_System->playSound(iter->second, NULL, 0, &m_CharChannel);
			return;
		}
	}
}

void SoundManager::SFXPause(string soundName, int channel)
{
	for (auto iter = m_GameSceneSFX.begin(); iter != m_GameSceneSFX.end(); iter++)
	{
		if (iter->first == soundName)
		{
			m_SFXChannel[channel]->setPaused(true);
		}
	}

}

void SoundManager::BgmChange(int num)
{
	m_BgmChannel->stop();
	switch (num)
	{
	case 0:
	{
		m_System->playSound(m_MainMenuBgm["TitleBGM"], NULL, 0, &m_BgmChannel);
	}
	break;
	case 1:
	{
		m_System->playSound(m_GameSceneBgm["GameBGM"], NULL, 0, &m_BgmChannel);
	}
	break;
	case 2:
	{
		m_System->playSound(m_BadEndingBgm["BadEndingBGM"], NULL, 0, &m_BgmChannel);
	}
	break;
	case 3:
	{
		m_System->playSound(m_NormalEndingBgm["NormalEndingBGM"], NULL, 0, &m_BgmChannel);
	}
	break;
	case 4:
	{
		m_System->playSound(m_HappyEndingBgm["HappyEndingBGM"], NULL, 0, &m_BgmChannel);
	}
	break;
	default:
		break;
	}
}
