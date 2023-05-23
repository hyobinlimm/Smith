#include "CCPch.h"
#include "UI.h"
#include "UIRenderer.h"
#include "ControllerEnum.h"
#include "UIPage.h"
#include "InputManager.h"
#include "SoundManager.h"

void UIPage::Awake()
{

}
void UIPage::Start()
{
	m_UIList = m_pGameObject->GetComponent<UI>()->GetUIList();
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type.find(L"Category") != wstring::npos)
		{
			m_MaxCategory++;
		}
		else if (_UI->GetUIData()->Type.find(L"Info") != wstring::npos)
		{
			m_MaxIndex++;
		}
	}

	m_MaxIndex =m_MaxIndex/ m_MaxCategory;

}
void UIPage::Update()
{


}
void UIPage::Change(int direction)
{

	/// <summary>
	/// 이거 여기서 하지 말고 컨트롤러에다가 달아놓는게 맞을듯
	/// </summary>
	/// <param name="direction"></param>
	//SoundManager::GetInstance()->SFXPlay("Menu_Move", 3);
	switch (direction)
	{
	case UP:
		if (m_Index <= 1)
		{
			m_Index = m_MaxIndex;
		}
		else
		{
			m_Index--;
		}
		break;
	case DOWN:
		if (m_Index >= m_MaxIndex)
		{
			m_Index = 1;
		}
		else
		{
			m_Index++;
		}
		break;
	case LEFT:
		if (m_Category <= 1)
		{
			m_Category = m_MaxCategory;
		}
		else
		{
			m_Category--;
		}
		m_Index = 1;
		break;
	case RIGHT:
		if (m_Category >= m_MaxCategory)
		{
			m_Category = 1;
		}
		else
		{
			m_Category++;
		}
		m_Index = 1;
		break;
	}
	UpdateCategory();
	UpdatePage();
}
void UIPage::UpdatePage()
{
	for (auto& obj : m_UIList)
	{
		bool IsPage = obj->GetUIData()->Type.find(L"Info") != wstring::npos;
		if (IsPage)
		{
			obj->GetUIData()->isDraw = false;
		}
	}
	for (auto& obj : m_UIList)
	{
		bool IsPage = obj->GetUIData()->Type.find(L"Info") != wstring::npos;
		bool CurrentPage = obj->GetUIData()->Data.find(to_wstring(m_Category) + to_wstring(0) + to_wstring(m_Index)) != wstring::npos;
		if (CurrentPage&&IsPage)
		{
			obj->GetUIData()->isDraw = true;
		}
	}
}

void UIPage::UpdateCategory()
{
	for (auto& obj : m_UIList)
	{
		bool IsCategory = obj->GetUIData()->Type.find(L"Category") != wstring::npos;
		if (IsCategory)
		{
			obj->GetUIData()->isDraw = false;
		}
	}
	for (auto& obj : m_UIList)
	{
		bool CurrentCategory = obj->GetUIData()->Data.find(L"00" + to_wstring(m_Category)) != wstring::npos;
	
		if (CurrentCategory && obj->GetUIData()->Type.find(L"Category") != wstring::npos)
		{
			obj->GetUIData()->isDraw = true;
		}
	}

	for (auto& obj : m_UIList)
	{
		if (obj->GetUIData()->Type.find(L"Frame") != wstring::npos)
		{
			obj->GetUIData()->isDraw = true;
		}
	}
	
}