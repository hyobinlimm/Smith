#include "CCPch.h"
#include "GameObject.h"
#include "Component.h"
#include "Controller.h"
#include "UI.h"
#include "SceneManager.h"
#include "WeaponQuest.h"
#include "QuestManager.h"
#include "Interactable.h"
#include "MeshRenderer.h"
#include "AIController.h"

/// <summary>
/// 우선 그 뭐냐 작업순서는 나중에 미루자
/// </summary>
/// 
/// 
void Interactable::Awake()
{

}

void Interactable::Start()
{
	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetComponent<Controller>() && obj->GetName() == L"Controller")
		{
			m_PlayerController = obj->GetComponent<Controller>();
			m_Weapon = obj->GetComponent<WeaponQuest>();
		}
		if (obj->GetComponent<AIController>() != nullptr)
		{
			m_Warrior = obj->GetComponent<AIController>();
		}
	}
	SetUI();
}

void Interactable::Update()
{

}

void Interactable::SetUI()
{
	wstring _name = L"";
	switch (m_Sequence)
	{
	case 0:
		_name = L"WeaponUI";
		break;
	case 1:
		_name = L"MaterialUI";
		break;
	case 2:
		_name = L"MeltingUI";
		break;
	case 3:
		_name = L"PouringUI";
		break;
	case 4:
		_name = L"HammeringUI";
		break;
	case 5:
		_name = L"EnchantSelectUI";
		break;
	case 6:
		_name = L"EnchantHitUI";
		break;
	case 7:
		_name = L"HandleUI";
		break;
	case 9:
		_name = L"QuestUI";
		break;
	default:
		break;
	}

	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetName() == _name)
		{
			m_UI = obj->GetComponent<UI>();
		}
	}
}

void Interactable::Interact()
{
	if (m_Weapon->GetCurrentSequence() == m_Sequence)
	{
		OpenMiniGame();
	}
	else if (m_Sequence == 1 && m_Weapon->GetCurrentSequence() == 0)
	{
		SetSequence(0);
		SetUI();
		OpenMiniGame();
	}
	else if (m_Sequence == 0 && m_Weapon->GetCurrentSequence() == 1)
	{
		SetSequence(1);
		SetUI();
		OpenMiniGame();
	}
	else if (m_Sequence == 5 && m_Weapon->GetCurrentSequence() == 6)
	{
		SetSequence(6);
		SetUI();
		OpenMiniGame();
	}
	else if (m_Sequence == 6 && m_Weapon->GetCurrentSequence() == 5)
	{
		SetSequence(5);
		SetUI();
		OpenMiniGame();
	}
	else if (m_Sequence == 7 && m_Weapon->GetCurrentSequence() == 8)
	{
		if (QuestManager::GetInstance()->GetClearedQuestCount() >= 5)
		{
			m_Warrior->MakeWarrior();
			m_Weapon->OpenReviewUI();
			m_PlayerController->SetMode(3);
		}
	}
	else if (m_Sequence == 7 && m_Weapon->GetCurrentSequence() == 9)
	{
		if (QuestManager::GetInstance()->GetClearedQuestCount() >= 5)
		{
			m_Warrior->DeleteWarrior();
			m_Weapon->SetNewQuest();
			m_PlayerController->OpenQuestUI();
			m_PlayerController->SetMode(2);
			m_Weapon->SetCurrentSequence(0);
		}

	}
	else
	{
		for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
		{
			if (obj->GetName() == L"Speech_bubble")
			{
				obj->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = true;
			}
		}
	}
}
void Interactable::OpenMiniGame()
{
	//여기!!!!!
	m_PlayerController->SetUI(m_UI);
	m_UI->OpenUI();
	m_PlayerController->CheckUI();
	m_PlayerController->TurnMainButtonUI(false);
	m_PlayerController->SetMode(2);
}