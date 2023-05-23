#include "CCPch.h"
#include "UIRenderer.h"
#include "WeaponQuest.h"
#include "SceneManager.h"
#include "UI.h"
#include "QuestUI.h"


void QuestUI::Awake()
{
	
}

void QuestUI::Start()
{
	for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (_obj->GetComponent<WeaponQuest>())
		{
			m_Quest = _obj->GetComponent<WeaponQuest>();
		}
	}
	m_UIList = m_pGameObject->GetComponent<UI>()->GetUIList();
}

void QuestUI::Update()
{
	
}

void QuestUI::QuestUISetup()
{
	for (auto& _UI : m_UIList)
	{
		if (m_Quest->GetQuestID() == L"" && _UI->GetUIData()->Type == L"Quest_Script"&& _UI->GetUIData()->Data==L"Q000")
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (_UI->GetUIData()->Data != m_Quest->GetQuestID()&&_UI->GetUIData()->Type==L"Quest_Script")
		{
			_UI->GetUIData()->isDraw = false;
		}
		else if (_UI->GetUIData()->Data == m_Quest->GetQuestID())
		{
			_UI->GetUIData()->isDraw = true;
		}
	}
}
