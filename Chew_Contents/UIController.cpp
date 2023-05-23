#include "CCPch.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "UIRenderer.h"
#include "ControllerEnum.h"
#include "UIController.h"

void UIController::Awake()
{

}

void UIController::Start()
{
	m_Scene = SceneManager::GetInstance();
	for (auto& _UI : m_Scene->m_CurScene->m_GameObjectList)
	{
		//ui type±× ¹¹³Ä 
		if (_UI->GetComponent<UIRenderer>())
		{
			if(_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"QuestUI")
			{
				m_QuestUI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"BookUI")
			{
				m_BookUI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"ButtonUI")
			{
				m_ButtonUI.push_back(_UI->GetComponent<UIRenderer>());
			}
		}
	}
}

void UIController::Update()
{

}

void UIController::OpenUI()
{
	m_Index = 1;
	m_Category = 1;
	switch (m_Type)
	{
	case QUEST:
		for (auto& _UI : m_QuestUI)
		{
			_UI->GetUIData()->isDraw = true;
		}
		break;
	case BOOK:
		for (auto& _UI : m_BookUI)
		{
			_UI->GetUIData()->isDraw = true;
		}
		break;
	default:
		break;
	}
	UpdatePage();
	for (auto& _UI : m_ButtonUI)
	{
		_UI->GetUIData()->isDraw = true;
	}
}

void UIController::CloseUI()
{

	switch (m_Type)
	{
	case QUEST:
		for (auto& _UI : m_QuestUI)
		{
			_UI->GetUIData()->isDraw = false;
		}
		break;
	case BOOK:
		for (auto& _UI : m_BookUI)
		{
			_UI->GetUIData()->isDraw = false;
		}
		break;
	default:
		break;
	}
	for (auto& _UI : m_ButtonUI)
	{
		_UI->GetUIData()->isDraw = false;
	}
}


void UIController::UpdatePage()
{
	switch (m_Type)
	{
	case QUEST:
		break;
	case BOOK:
		for (auto& _UI : m_BookUI)
		{
			if (!(_UI->GetUIData()->Name.find(L"BookPage") == wstring::npos))
			{
				_UI->GetUIData()->isDraw = false;
			}
			if (!(_UI->GetUIData()->Name.find(L"BookCategory") == wstring::npos))
			{
				_UI->GetUIData()->isDraw = false;
			}
			if (_UI->GetUIData()->Name == CheckIndex())
			{
				_UI->GetUIData()->isDraw = true;
			}
			if (_UI->GetUIData()->Name == CheckCategory())
			{
				_UI->GetUIData()->isDraw = true;
			}
		}
		break;
	default:
		break;
	}
}

wstring UIController::CheckIndex()
{
	wstring _type;
	switch (m_Type)
	{
	case QUEST:
		break;
	case BOOK:

		_type = L"BookPage";

		break;
	default:
		break;
	}
	wstring _index;
	wstring _category;
	_index = to_wstring(m_Index);
	_category = to_wstring(m_Category);


	wstring _final = _type + _category + _index;



	return _final;

}


std::wstring UIController::CheckCategory()
{
	wstring _type;
	_type = L"BookCategory";

	return _type + to_wstring(m_Category);

}

void UIController::Move(int dir)
{
	switch (dir)
	{

	case RIGHT:
		m_Category++;
		if (m_Category >= 5)
		{
			m_Category = 5;
		}

		break;
	case LEFT:
		m_Category--;
		if (m_Category <= 1)
		{
			m_Category = 1;
		}
		break;
	case UP:
		m_Index--;
		if (m_Index <= 1)
		{
			m_Index = 1;
		}
		break;
	case DOWN:
		m_Index++;
		if (m_Index >= 2)
		{
			m_Index = 2;
		}
		break;
	default:
		break;
	}
	UpdatePage();
}



void UIController::SetUI(int type)
{
	m_Type = type;
}

int UIController::GetUI()
{
	return m_Type;
}

