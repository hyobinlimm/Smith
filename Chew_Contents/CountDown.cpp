#include "CCPch.h"
#include "UIRenderer.h"
#include "GameTimer.h"
#include "UI.h"
#include "SceneManager.h"
#include "CountDown.h"

void CountDown::Awake()
{

}

void CountDown::Start()
{

	for (auto& _UI : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (_UI->GetComponent<UIRenderer>())
		{
			if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"CountDownUI")
			{
				m_UIList.push_back(_UI->GetComponent<UIRenderer>());
			}
		}

	}
}

void CountDown::Update()
{
	m_PrevTimer = m_CountdownTimer;
	m_CountdownTimer += GameTimer::GetInstance()->DeltaTime();


	if (m_IsTimer)
	{
		if (m_CountdownTimer > 4)
		{
			m_IsTimer = false;
			m_CountdownTimer = 0;
			m_PrevTimer = 0;
		}
		else
		{
			if (m_PrevTimer != (int)m_CountdownTimer)
			{
				UpdateCountdownTimerUI();
			}
		}
	}


}

void CountDown::TurnEndUI()
{
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Data.find(to_wstring(0)) != wstring::npos&&_UI->GetUIData()->Type==L"Close")
		{
			if (_UI->GetUIData()->isDraw)
			{
				_UI->GetUIData()->isDraw = false;
			}
			else
			{
				_UI->GetUIData()->isDraw = true;
			}
			
		}
		else
		{
			_UI->GetUIData()->isDraw = false;
		}
	}
}

void CountDown::SetIsTimer(bool val)
{
	m_IsTimer = val;
	m_CountdownTimer = 0;
	m_PrevTimer = 0;
	UpdateCountdownTimerUI();

}

void CountDown::UpdateCountdownTimerUI()
{
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Data.find(to_wstring(3 - (int)m_CountdownTimer)) != wstring::npos && _UI->GetUIData()->Type == L"Count")
		{
			_UI->GetUIData()->isDraw = true;
		}
		else
		{
			_UI->GetUIData()->isDraw = false;
		}
	}
}

