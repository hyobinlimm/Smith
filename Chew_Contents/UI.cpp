#include "CCPch.h"
#include "UIRenderer.h"
#include "UI.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "SoundManager.h"

void UI::Awake()
{
	m_Scene = SceneManager::GetInstance();
	//위에서 씬매니저의 인스턴스도 가져왔지?


	SetUIList(m_pGameObject->GetName());

}

void UI::Start()
{
	

}

void UI::Update()
{

}

void UI::SetUIList(wstring name)
{
	for (auto& _UI : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (_UI->GetComponent<UIRenderer>())
		{
			if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == name)
			{
				m_UIList.push_back(_UI->GetComponent<UIRenderer>());
			}
		}
	}
}

void UI::SwitchUI()
{
	for (auto& _UI : m_UIList)
	{
		_UI->GetUIData()->isDraw = !m_IsOpen;
	}
	m_IsOpen = !m_IsOpen;
}

bool UI::IsOpen()
{
	return m_IsOpen;
}
void UI::OpenUI()
{
	SceneManager::GetInstance()->m_CurScene->m_HimeEngine->SetFireEffect(false);
	for (auto& _UI : m_UIList)
	{
		//#사운드
		//이것도 여기가 아니라 컨트롤러 부분에서 ㄱ
		/*SoundManager::GetInstance()->BgmSoundControl(0.2f);
		SoundManager::GetInstance()->SFXPlay("QuestPaper_Open", 0);*/
		_UI->GetUIData()->isDraw = true;
	}
	m_IsOpen = true;
}

void UI::CloseUI()
{
	SceneManager::GetInstance()->m_CurScene->m_HimeEngine->SetFireEffect(true);
	for (auto& _UI : m_UIList)
	{
		SoundManager::GetInstance()->BgmSoundControl(0.5f);
		//SoundManager::GetInstance()->SFXPlay("Menu_Select", 6);
		_UI->GetUIData()->isDraw = false;
	}
	m_IsOpen = false;
}

//빈 옵젝을 만들고 여기에 UI를 달고 UI가 한 UI를가지고 있음
//????