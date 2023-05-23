#include "CCPch.h"
#include "UI.h"
#include "SceneManager.h"
#include "QuestManager.h"
#include "WeaponQuest.h"
#include "GameTimer.h"
#include "UIRenderer.h"
#include "InputManager.h"
#include "Environment.h"
#include "SoundManager.h"

void Environment::Awake()
{
	SetDebugInfo();
}

void Environment::Start()
{
	SceneManager::GetInstance()->m_CurScene->m_SceneDate = 1;
	for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (_obj->GetName() == L"MainUI")
		{
			m_UIList = _obj->GetComponent<UI>()->GetUIList();
		}
		else if (_obj->GetName() == L"Controller")
		{
			m_Quest = _obj->GetComponent<WeaponQuest>();
		}
		else if (_obj->GetName() == L"EndingUI")
		{
			m_EndingUIList = _obj->GetComponent<UI>()->GetUIList();
		}
	}
	for (auto& _UI : m_UIList)
	{
		_UI->GetUIData()->isDraw = true;
	}
	SetFameUI();
	SetDayUI();
	SetDaySceneUI();
	m_UITimer = 0;
}

void Environment::SetFameUI()
{
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"Star" && _UI->GetUIData()->Data == to_wstring(CalculateFame()))
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (_UI->GetUIData()->Type == L"Star")
		{
			_UI->GetUIData()->isDraw = false;

		}
	}
}

void Environment::Update()
{
	m_Time += GameTimer::GetInstance()->DeltaTime();
	m_UITimer += GameTimer::GetInstance()->DeltaTime();
	SetClockUI();
	CountDay();
	UpdateDebugInfo();
	if (m_IsUITimer)
	{
		if (m_UITimer >= 3)
		{
			if (m_GameEND)
			{
				CloseEndingUI();
				exit(0);
		/*		m_TimePerDay = 420;
				m_Fame = 8;
				m_Time = 0;
				m_Date = 1;
				m_IsUITimer = false;
				m_UITimer = 0;

				bool m_GameEND = false;
				m_GameEND = false;
				m_IsUITimer = false;
				shared_ptr<SceneManager> m_Scene = SceneManager::GetInstance();
				SceneManager::GetInstance()->LoadScene(L"TitleScene");
				SceneManager::GetInstance()->m_CurScene->Awake();
				SceneManager::GetInstance()->m_CurScene->Start();*/
			}

			for (auto& _UI : m_UIList)
			{
				if (_UI->GetUIData()->Type == L"DayScene" && _UI->GetUIData()->Data == to_wstring(m_Date))
				{
					_UI->GetUIData()->isDraw = false;
				}
			}
			SceneManager::GetInstance()->m_CurScene->m_HimeEngine->SetFireEffect(true);
			m_IsUITimer = false;
			m_UITimer = 0;
		}

	}

	if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::Key_P) == KeyState::Down)
	{
		SetFame(1);
		SetFameUI();
	}
	if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::Key_O) == KeyState::Down)
	{
		SetFame(-1);
		SetFameUI();
	}
	if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F6) == KeyState::Down)
	{
		SetEndingUI();
	}
}

void Environment::SetFame(int val)
{

	m_Fame += val;
	if (m_Fame > 20)
	{
		m_Fame = 20;
	}
	else if (m_Fame < 0)
	{
		m_Fame = 0;
	}
}

void Environment::CountDay()
{
	bool IsMaxDayOver = m_Date >= 3;

	if (m_Time >= m_TimePerDay)
	{
		if (!IsQuestProcessing())
		{
			if (IsMaxDayOver)
			{
				SetEndingUI();

			}
			else
			{
				m_Date++;
				SceneManager::GetInstance()->m_CurScene->m_SceneDate = m_Date;
			}
			SetDayUI();
			SetDaySceneUI();
			m_Time = 0;
		}
		else
		{
			m_Time = m_TimePerDay;
		}
	}
}

void Environment::SetEndingUI()
{

	//#여기
	SceneManager::GetInstance()->m_CurScene->m_HimeEngine->SetFireEffect(false);
	for (auto& _UI : m_EndingUIList)
	{
		if (_UI->GetUIData()->Type == L"Ending" && _UI->GetUIData()->Data.find(to_wstring(CalculateFame())) != wstring::npos)
		{
			_UI->GetUIData()->isDraw = true;
			InputManager::GetInstance()->SetIsInputAble(false);
		}
	}
	if (CalculateFame() == 0 || CalculateFame() == 1)
	{
		SoundManager::GetInstance()->BgmSetting("BadEndingBGM");
	}
	else if (CalculateFame() == 2 || CalculateFame() == 3)
	{
		SoundManager::GetInstance()->BgmSetting("NormalEndingBGM");
	}
	else if (CalculateFame() == 4 || CalculateFame() == 5)
	{
		SoundManager::GetInstance()->BgmSetting("HappyEndingBGM");
	}
	m_IsUITimer = true;
	m_UITimer = 0;
	m_GameEND = true;
}

void Environment::CloseEndingUI()
{
	for (auto& _UI : m_EndingUIList)
	{
		if (_UI->GetUIData()->Type == L"Ending" && _UI->GetUIData()->Data.find(to_wstring(CalculateFame())) != wstring::npos)
		{
			_UI->GetUIData()->isDraw = false;
			InputManager::GetInstance()->SetIsInputAble(true);
		}
	}
}

void Environment::SetDayUI()
{




	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"Day" && _UI->GetUIData()->Data == to_wstring(m_Date))
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (_UI->GetUIData()->Type == L"Day")
		{
			_UI->GetUIData()->isDraw = false;
		}
	}



}

void Environment::SetDaySceneUI()
{

	//#여기
	SceneManager::GetInstance()->m_CurScene->m_HimeEngine->SetFireEffect(false);
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"DayScene" && _UI->GetUIData()->Data == to_wstring(m_Date))
		{
			_UI->GetUIData()->isDraw = true;

		}
		else if (_UI->GetUIData()->Type == L"DayScene")
		{
			_UI->GetUIData()->isDraw = false;

		}
	}
	QuestManager::GetInstance()->SetClearedQuestCount(-(QuestManager::GetInstance()->GetClearedQuestCount()));
	m_IsUITimer = true;
	m_UITimer = 0;
}

bool Environment::IsQuestProcessing()
{
	if (m_Quest->GetQuestID() == L"")
	{
		return false;
	}
	return true;
}

int Environment::CalculateFame()
{
	if (0 <= GetFame() && GetFame() < 1)
	{
		return 0;
	}
	else if (1 <= GetFame() && GetFame() < 5)
	{
		return 1;
	}
	else if (5 <= GetFame() && GetFame() < 10)
	{
		return 2;
	}
	else if (10 <= GetFame() && GetFame() < 14)
	{
		return 3;
	}
	else if (14 <= GetFame() && GetFame() < 18)
	{
		return 4;
	}
	else if (18 <= GetFame())
	{
		return  5;
	}
}



void Environment::SetDebugInfo()
{
	m_DebugInfo = make_shared<TextData>();
	m_DebugInfo->IDName = L"Environment";
	m_DebugInfo->text = L"Scene Timer : %f\nScene Date : %f";
	m_DebugInfo->color = { 1.f, 1.f, 1.f, 1.f };
	m_DebugInfo->FontSize = 2.f;
	m_DebugInfo->x = 50;
	m_DebugInfo->y = 900;
	m_DebugInfo->value = 0;
	m_DebugInfo->value2 = 0;
	m_DebugInfo->isDraw = false;
	SceneManager::GetInstance()->m_CurScene->m_stringList.emplace_back(m_DebugInfo);
}

void Environment::UpdateDebugInfo()
{
	m_DebugInfo->value = m_Time;
	m_DebugInfo->value2 = m_Date;
}

void Environment::SetClockUI()
{
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"Clock")
		{
			//6이 0
			_UI->GetUIData()->rotate = m_Time / m_TimePerDay * 6;
		}
	}
}

