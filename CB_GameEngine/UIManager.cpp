#include "CBpch.h"
#include "UIManager.h"

shared_ptr<UIManager> UIManager::g_UIInstance = nullptr;

std::shared_ptr<UIManager> UIManager::GetInstance()
{
	if (g_UIInstance == nullptr)
		g_UIInstance = make_shared<UIManager>();

	return g_UIInstance;
}

void UIManager::Initialize()
{
	m_UIData = make_shared<UIDataTable>();
	m_UIData->DeserializeFromFile("../Resource/DataTable/UITestData2.json");

	for (size_t i = 0; i < m_UIData->m_DataCount; i++)
	{
		shared_ptr<UIData> _NewUIData = make_shared<UIData>();
		_NewUIData->isDraw = false;
		_NewUIData->Name = m_UIData->m_Name[i];
		_NewUIData->UIType = m_UIData->m_UIType[i];
		_NewUIData->Data = m_UIData->m_Data[i];
		_NewUIData->Type = m_UIData->m_Type[i];
		_NewUIData->FileName = m_UIData->m_FileName[i];

		if (m_UIData->m_UIType[i] == L"MainUI")
		{
			m_MainUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"TestUI")
		{
			m_TestUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"QuestUI")
		{
			m_QuestUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"MonsterUI")
		{
			m_MonsterUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"ReviewUI")
		{
			m_ReviewUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"WeaponUI")
		{
			m_WeaponUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"MaterialUI")
		{
			m_MaterialUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"MeltingUI")
		{
			if (m_UIData->m_Type[i] == L"RecogAre")
			{
				_NewUIData->isMovable = true;
			}
			else if (m_UIData->m_Type[i] == L"PlayerBar")
			{
				_NewUIData->isMovable = true;
			}
			m_MeltingUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"PouringUI")
		{
			if (m_UIData->m_Type[i] == L"RecogAre")
			{
				_NewUIData->isMovable = true;
			}
			else if (m_UIData->m_Type[i] == L"PlayerBar")
			{
				_NewUIData->isMovable = true;
			}
			m_PouringUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"HammeringUI")
		{
			if (m_UIData->m_Type[i] == L"RecogAre")
			{
				_NewUIData->isMovable = true;
			}
			else if (m_UIData->m_Type[i] == L"PlayerBar")
			{
				_NewUIData->isMovable = true;
			}
			m_HammeringUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"EnchantHitUI")
		{
			if (m_UIData->m_Type[i] == L"RecogAre")
			{
				_NewUIData->isMovable = true;
			}
			else if (m_UIData->m_Type[i] == L"PlayerBar")
			{
				_NewUIData->isMovable = true;
			}
			m_EnchantHitUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"EnchantSelectUI")
		{
			m_EnchantSelectUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"HandleUI")
		{
			m_HandleUI.emplace_back(_NewUIData);
		}

		else if (m_UIData->m_UIType[i] == L"CountDownUI")
		{
			m_CountDownUI.emplace_back(_NewUIData);
		}

		else if (m_UIData->m_UIType[i] == L"TitleUI")
		{
			m_TitleUI.emplace_back(_NewUIData);
		}
		else if (m_UIData->m_UIType[i] == L"CreditUI")
		{
			m_CreditUI.emplace_back(_NewUIData);
		}
				
		else if (m_UIData->m_UIType[i] == L"EndingUI")
		{
		m_EndingUI.emplace_back(_NewUIData);
		}
	}
}
