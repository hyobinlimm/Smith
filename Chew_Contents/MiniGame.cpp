#include "CCPch.h"
#include "GameObject.h"
#include "UIRenderer.h"
#include "SceneManager.h"
#include "HimeRendererStructs.h"
#include "WeaponQuest.h"
#include "ControllerEnum.h"
#include "MiniGame.h"

void MiniGame::Awake()
{

}

void MiniGame::Start()
{
	m_Scene = SceneManager::GetInstance();
	for (auto& _UI : m_Scene->m_CurScene->m_GameObjectList)
	{
		if (_UI->GetComponent<UIRenderer>())
		{
			if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"WeaponUI")
			{
				m_WeaponUI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"MaterialUI")
			{
				m_MaterialUI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"Minigame1UI")
			{
				m_Minigame1UI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"Minigame2UI")
			{
				m_Minigame2UI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"Minigame3UI")
			{
				m_Minigame3UI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"EnchantUI")
			{
				m_EnchantUI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"Minigame4UI")
			{
				m_Minigame4UI.push_back(_UI->GetComponent<UIRenderer>());
			}
			else if (_UI->GetComponent<UIRenderer>()->GetUIData()->UIType == L"HandleUI")
			{
				m_HandleUI.push_back(_UI->GetComponent<UIRenderer>());
			}
		}
	}
}

void MiniGame::Update()
{
	if (m_GameType == HIT && m_CurrentWork == POURING)
	{
		//if (m_MoveBar->GetUIData()->top <= 50)
		//{
		//	//m_MoveBar->GetUIData()->top += m_Speed;
		//	//m_MoveBar->GetUIData()->bottom += m_Speed;
		//}
	}
	if (m_GameType == KEEP)
	{
		//랜덤 방향
		//m_FishBar->GetUIData()->left += m_Speed;
		//m_FishBar->GetUIData()->right += m_Speed;
		//m_MoveBar->GetUIData()->left -= m_Speed / 2;
		//m_MoveBar->GetUIData()->right -= m_Speed / 2;
	}
}

void MiniGame::SetUI()
{
	switch (m_CurrentWork)
	{
	case WEAPONSELECT:
		m_GameType = SELECT;
		m_CurMini = m_WeaponUI;
		m_IndexMax = 2;
		m_CategoryMax = 4;
		break;
	case MATERIALSELECT:
		m_GameType = SELECT;
		m_CurMini = m_MaterialUI;
		m_IndexMax = 4;
		m_CategoryMax = 2;
		break;
	case MELTING:
		m_GameType = HIT;
		m_CurMini = m_Minigame1UI;
		break;
	case POURING:
		m_GameType = HIT;
		m_CurMini = m_Minigame2UI;
		break;
	case HAMMERING:
		m_GameType = HIT;
		m_CurMini = m_Minigame3UI;
		break;
	case ENCHANTSELECT:
		m_GameType = SELECT;
		m_CurMini = m_EnchantUI;
		m_IndexMax = 4;
		m_CategoryMax = 1;
		break;
	case ENCHANTHIT:
		m_GameType = HIT;
		m_CurMini = m_Minigame4UI;
		break;
	case HANDGUARDING:
		m_GameType = SELECT;
		m_CurMini = m_HandleUI;
		m_IndexMax = 3;
		m_CategoryMax = 1;
		break;
	default:
		break;
	}
	for (auto& _UI : m_CurMini)
	{
		_UI->GetUIData()->isDraw = true;
	}

	if (m_GameType == SELECT)
	{
		DrawSelect();
	}

	else if (m_GameType == HIT)
	{
		for (auto& _UI : m_CurMini)
		{
			if (_UI->GetUIData()->isMovable)
			{
				m_MoveBar = _UI;
			}
		}
	}

	else if (m_GameType == KEEP)
	{
		for (auto& _UI : m_CurMini)
		{
			if (_UI->GetUIData()->isMovable)
			{
				m_FishBar = _UI;
			}
		}
	}

}

void MiniGame::DrawSelect()
{
	//선택창 기준 오른쪽
	for (auto& _UI : m_CurMini)
	{
		if (_UI->GetUIData()->Type.find(L"ItemInfo") != wstring::npos)
		{
			_UI->GetUIData()->isDraw = false;
		}

		if (_UI->GetUIData()->Type.find(L"ItemInfo") != wstring::npos && _UI->GetUIData()->Data.find(to_wstring(m_Category) + L"0" + to_wstring(m_Index)) != wstring::npos)
		{
			_UI->GetUIData()->isDraw = true;
		}
	}

	//선택창 기준 왼쪽
	for (auto& _UI : m_CurMini)
	{
		if (_UI->GetUIData()->Type.find(L"ItemImage") != wstring::npos)
		{
			_UI->GetUIData()->isDraw = false;
		}
		if (_UI->GetUIData()->Type.find(L"ItemImage") != wstring::npos && _UI->GetUIData()->Data.find(to_wstring(m_Category) + L"0" + to_wstring(m_Index)) != wstring::npos)
		{
			_UI->GetUIData()->isDraw = true;
		}
	}
}

void MiniGame::DrawSelected(int count)
{
	//선택칸 안
	if (count == 0)
	{
		/*	for (auto& _UI : m_CurMini)
			{
				if (_UI->GetUIData()->Type.find(L"ItemImage_Select" + to_wstring(count)) != wstring::npos)
				{
					_UI->GetUIData()->isDraw = false;
				}
				if (_UI->GetUIData()->Type.find(L"ItemImage_Select" + to_wstring(count)) != wstring::npos && _UI->GetUIData()->Data.find(to_wstring(m_Category) + L"0" + to_wstring(m_Index)) != wstring::npos)
				{
					_UI->GetUIData()->isDraw = true;
				}
			}*/
	}
	for (auto& _UI : m_CurMini)
	{
		if (_UI->GetUIData()->Type.find(L"ItemImage_Select" + to_wstring(count)) != wstring::npos)
		{
			_UI->GetUIData()->isDraw = false;
		}
		if (_UI->GetUIData()->Type.find(L"ItemImage_Select" + to_wstring(count)) != wstring::npos && _UI->GetUIData()->Data.find(to_wstring(m_Category) + L"0" + to_wstring(m_Index)) != wstring::npos)
		{
			_UI->GetUIData()->isDraw = true;
		}
	}
}

void MiniGame::DrawHit()
{

}

void MiniGame::Move(int direction)
{
	if (m_GameType == SELECT)
	{
		switch (direction)
		{
		case UP:
			m_Index--;
			if (m_Index <= 1) m_Index = 1;
			break;
		case DOWN:
			m_Index++;
			if (m_Index >= m_IndexMax) m_Index = m_IndexMax;
			break;
		case LEFT:
			m_Category--;
			if (m_Category <= 1) m_Category = 1;
			break;
		case RIGHT:
			m_Category++;
			if (m_Category >= m_CategoryMax) m_Category = m_CategoryMax;
			break;
		default:
			break;
		}
		DrawSelect();
	}
}

void MiniGame::Action()
{
	if (m_GameType == SELECT)
	{
		switch (m_Count)
		{
		case 0:
			m_Selected[0] = m_Category * 100 + m_Index;
			m_Count++;
			break;
		case 1:
			if (m_CurrentWork == MATERIALSELECT)
			{
				m_Selected[1] = m_Category * 100 + m_Index;
				m_Count++;
			}
			break;
		default:
			break;
		}
		DrawSelected(m_Count);
	}
	else if (m_GameType == HIT)
	{

	}
}

void MiniGame::Cancel()
{
	if (m_GameType == SELECT)
	{

		if (m_GameType == SELECT)
		{
			switch (m_Count)
			{
			case 1:
				m_Selected[0] = 0;
				m_Count--;
				break;
			case 2:
				if (m_CurrentWork == MATERIALSELECT)
				{
					m_Selected[1] = 0;
					m_Count--;
				}
				break;
			default:
				break;
			}
			DrawSelected(m_Count);
		}
	}
	else if (m_GameType == HIT)
	{

	}
}

void MiniGame::Confirm()
{
	if (m_GameType == SELECT)
	{
		m_CurrentWork++;
	}
	else if (m_GameType == HIT)
	{
		switch (m_CurrentWork)
		{
		case POURING:
			//if (m_MoveBar->GetUIData()->top <= 33 || m_MoveBar->GetUIData()->top >= 27)
			//{
			//	//?
			//}
			break;
		case HAMMERING:
			/*if (m_TryCount <= 4)
			{
				if (m_MoveBar->GetUIData()->left <= 55 || m_MoveBar->GetUIData()->left >= 45)	m_Count++;

			}*/
			//else if (m_TryCount >= 5)
			//{
			//	//횟수 보내고 종료
			//}
			m_TryCount++;
			break;
		case ENCHANTHIT:
			if (m_TryCount <= 2)
			{
				/*if (m_MoveBar->GetUIData()->left <= m_RandCount[m_TryCount] + 5 || m_MoveBar->GetUIData()->left >= m_RandCount[m_TryCount])
				{
					m_Count++;
				}
				m_TryCount++;*/
			}
			else
			{
				//데이터 보내
			}
			break;

		default:
			break;
		}

	}
	else if (m_GameType == KEEP)
	{
		//m_MoveBar->GetUIData()->left += m_Speed;
		//m_MoveBar->GetUIData()->right += m_Speed;
	}
}

void MiniGame::Quit()
{
	for (auto& _UI : m_CurMini)
	{
		_UI->GetUIData()->isDraw = false;
	}


	//그냥 나가는거랑 완료해서 나가거랑 다르게
	/*
	m_CurrentWork++;
	Reset();*/
}

void MiniGame::Reset()
{
	m_CurMini.clear();
	vector<shared_ptr<UIRenderer>>().swap(m_CurMini);
	m_GameType = 0;
	m_Category = 1;
	m_Index = 1;
	m_Count = 0;
	m_IndexMax = 0;
	m_CategoryMax = 0;
	fill_n(m_Selected, 2, 0);
}
