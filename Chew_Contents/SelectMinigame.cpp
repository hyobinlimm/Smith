#include "CCPch.h"
#include "UI.h"
#include "UIPage.h"
#include "WeaponQuest.h"
#include "UIRenderer.h"
#include "SelectMinigame.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "SoundManager.h"

void SelectMinigame::Awake()
{

}

void SelectMinigame::Start()
{
	m_UIList = m_pGameObject->GetComponent<UI>()->GetUIList();
	m_UIPage = m_pGameObject->GetComponent<UIPage>();
	for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (_obj->GetComponent<WeaponQuest>())
		{
			m_Weapon = _obj->GetComponent<WeaponQuest>();
		}
	}

}

void SelectMinigame::Update()
{

}
void SelectMinigame::Select()
{
	if (m_Count <= 2)
	{
		//Weapon 의 W, Enchant의 E 이런 글자 가져옴
		auto FirstCharacter = m_UIList[0]->GetUIData()->UIType.substr(0, 1);
		if (FirstCharacter != L"M" && m_Count == 2)
		{
			return;
		}
		//Category + 0 + Index = 301이라든가
		m_Weapon->SetDataW(to_wstring(m_UIPage->GetCategory()) + to_wstring(0) + to_wstring(m_UIPage->GetIndex()), FirstCharacter, m_Count);

		for (auto& _UI : m_UIList)
		{

			if (_UI->GetUIData()->Type.find(L"Select" + to_wstring(m_Count - 1)) != wstring::npos)
			{

			}
			else if (_UI->GetUIData()->Type.find(L"Select" + to_wstring(m_Count)) != wstring::npos
				&& _UI->GetUIData()->Data.find(to_wstring(m_UIPage->GetCategory()) + to_wstring(0) + to_wstring(m_UIPage->GetIndex())) != wstring::npos)
			{
				_UI->GetUIData()->isDraw = true;
			}
		}

	}
	m_Count++;
}

void SelectMinigame::Cancel()
{
	if (2 <= m_Count)
	{
		auto FirstCharacter = m_UIList[0]->GetUIData()->UIType.substr(0, 1);
		m_Weapon->SetDataW(L"", FirstCharacter, m_Count - 1);

		//이거 빈칸으로 만들기

		for (auto& _UI : m_UIList)
		{
			if (_UI->GetUIData()->Type.find(L"Select" + to_wstring(m_Count - 1)) != wstring::npos)
			{
				_UI->GetUIData()->isDraw = false;
			}

			if (_UI->GetUIData()->Data.find(L"000") != wstring::npos && _UI->GetUIData()->Type.find(L"Select" + to_wstring(m_Count - 1)))
			{
				_UI->GetUIData()->isDraw = true;
			}


		}
		m_Count--;
	}
}

void SelectMinigame::SelectUISetUP()
{
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type.find(L"Select") != wstring::npos)
		{
			_UI->GetUIData()->isDraw = false;
		}
		if (_UI->GetUIData()->Data.find(L"000") != wstring::npos)
		{
			_UI->GetUIData()->isDraw = true;
		}
	}
	m_Count = 1;

	auto FirstCharacter = m_UIList[0]->GetUIData()->UIType.substr(0, 1);
	//Category + 0 + Index = 301이라든가
	m_Weapon->SetDataW(L"", FirstCharacter, 1);
	m_Weapon->SetDataW(L"", FirstCharacter, 2);

}

bool SelectMinigame::Confirm()
{
	auto FirstCharacter = m_UIList[0]->GetUIData()->UIType.substr(0, 1);
	if (FirstCharacter != L"M" && m_Count == 2)
	{
		//SoundManager::GetInstance()->SFXPlay("QuestPaper_Close", 4);
		m_pGameObject->GetComponent<UI>()->CloseUI();

		return true;
	}
	else if (m_Count >= 3)
	{
		//SoundManager::GetInstance()->SFXPlay("QuestPaper_Close", 4);
		m_pGameObject->GetComponent<UI>()->CloseUI();
		return true;
	}
	return false;

}
