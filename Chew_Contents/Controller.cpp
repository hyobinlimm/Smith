#include "CCPch.h"
#include "GameObject.h"
#include "ControllerEnum.h"
#include "Component.h"
#include "Movement.h"
#include "UI.h"
#include "UIRenderer.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "UIPage.h"
#include "SelectMinigame.h"
#include "HitMinigame.h"
#include "QuestUI.h"
#include "WeaponQuest.h"
#include "BoxCollider.h"
#include "Interactable.h"
#include "Controller.h"
#include "MeshRenderer.h"
#include "AnimState.h"
#include "SoundManager.h"

void Controller::Start()
{
	m_Input = InputManager::GetInstance();
	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetName() == L"QuestUI")
		{
			m_QuestUI = obj->GetComponent<UI>();;
		}
		else if (obj->GetName() == L"MonsterUI")
		{
			m_MonsterUI = obj->GetComponent<UI>();
		}
		else if (obj->GetName() == L"Smith")
		{
			m_CharacterMovement = obj->GetComponent<Movement>();
		}
		else if (obj->GetName() == L"MainUI")
		{
			m_MainUI = obj->GetComponent<UI>();
		}
	}

	m_Count = 0;
	SetTestUI();

}


void Controller::TurnMainButtonUI(bool val)
{
	for (auto& _UI : m_MainUI->GetUIList())
	{
		if (_UI->GetUIData()->Type == L"Button")
		{
			_UI->GetUIData()->isDraw = val;
		}
	}
}

void Controller::Update()
{
	if (InputManager::GetInstance()->GetIsInputAble())
	{
		KeyUpdate();
	}

	m_DebugInfo->value = m_ControlMode;
	m_DebugInfo2->textValue = m_CharacterMovement->m_pGameObject->GetName();
}

void Controller::Awake()
{
	SetDebugInfo();
}

void Controller::SetDebugInfo()
{

	m_DebugInfo = make_shared<TextData>();
	m_DebugInfo->IDName = L"ControllerMode";
	m_DebugInfo->text = L"Current GameMode : %f";
	m_DebugInfo->color = { 1.f, 1.f, 1.f, 1.f };
	m_DebugInfo->FontSize = 2.f;
	m_DebugInfo->x = 50;
	m_DebugInfo->y = 280;
	m_DebugInfo->value = m_ControlMode;
	m_DebugInfo->isDraw = false;
	SceneManager::GetInstance()->m_CurScene->m_stringList.emplace_back(m_DebugInfo);


	m_DebugInfo2 = make_shared<TextData>();
	m_DebugInfo2->IDName = L"CharacterMove";
	m_DebugInfo2->text = L"Current MovingObject : % ls";
	m_DebugInfo2->color = { 1.f, 1.f, 1.f, 1.f };
	m_DebugInfo2->FontSize = 2.f;
	m_DebugInfo2->x = 600;
	m_DebugInfo2->y = 280;
	m_DebugInfo2->textValue = L"";
	m_DebugInfo2->isDraw = false;
	SceneManager::GetInstance()->m_CurScene->m_stringList.emplace_back(m_DebugInfo2);
}

void Controller::SetController()
{

}


void Controller::SetMode(int mode)
{
	m_ControlMode = mode;
}

void Controller::SetTestUI()
{
	wstring _name = L"";
	switch (m_Count)
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
	default:
		break;
	}

	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetName() == _name)
		{
			m_TestingUI = obj->GetComponent<UI>();
		}
	}

}

void Controller::KeyUpdate()
{
	ZKeyPressed();
	XKeyPressed();
	CKeyPressed();
	SpaceBarDowned();
	MovePressed();
	MoveDown();
	TestKeyDown();
	SpaceBarPressed();
	MoveKeyUp();
}

void Controller::SetUI(shared_ptr<UI> ui)
{
	m_UI = ui;
}

void Controller::ZKeyPressed()
{
	if (!(m_Input->GetCurrentKeyState(VirtualKey::Key_Z) == KeyState::Down)) return;
	TRACE("Z눌림\n");
	switch (m_ControlMode)
	{
	case CharacterMode:
		//여기같은 곳에서 해야함 ㅇㅇ space는
		SoundManager::GetInstance()->SFXPlay("QuestPaper_Open",3);
		OpenQuestUI();

		break;
	case UIMode:

		break;
	case MiniGameMode:
		if (m_Select)
		{
			SoundManager::GetInstance()->SFXPlay("Menu_Select", 6);
			m_Select->Select();
		}
		break;
	default:
		break;
	}
}

void Controller::OpenQuestUI()
{
	if (m_UI == m_MonsterUI || m_UI == nullptr)
	{
		m_QuestUI->OpenUI();
		m_ControlMode = UIMode;
		TurnMainButtonUI(false);
		m_UI = m_QuestUI;
		m_QuestUI->m_pGameObject->GetComponent<QuestUI>()->QuestUISetup();
		CheckUI();
	}
}

void Controller::XKeyPressed()
{
	if (!(m_Input->GetCurrentKeyState(VirtualKey::Key_X) == KeyState::Down)) return;
	switch (m_ControlMode)
	{
	case CharacterMode:
		SoundManager::GetInstance()->SFXPlay("MonsterBook_Open", 3);
		OpenMonsterBookUI();


		break;
	case UIMode:

		break;
	case MiniGameMode:
		if (m_Select)
		{
			m_Select->Cancel();
		}
		break;
	default:
		break;
	}
}

void Controller::OpenMonsterBookUI()
{
	if (m_UI == m_MonsterUI || m_UI == nullptr)
	{
		m_MonsterUI->OpenUI();
		m_UI = m_MonsterUI;
		m_ControlMode = UIMode;
		TurnMainButtonUI(false);
		CheckUI();

	}
}
void Controller::CKeyPressed()
{
	if (!(m_Input->GetCurrentKeyState(VirtualKey::Key_C) == KeyState::Down)) return;
	switch (m_ControlMode)
	{
	case CharacterMode:
		break;
	case UIMode:
		if (m_UI)
		{
			SoundManager::GetInstance()->SFXPlay("QuestPaper_Close", 7);
			m_UI->CloseUI();
			m_ControlMode = CharacterMode;
			m_UI = nullptr;
			CheckUI();
			TurnMainButtonUI(true);
		}

		break;
	case MiniGameMode:
		if (m_UI)
		{
			m_UI->CloseUI();
			m_ControlMode = CharacterMode;
			m_UI = nullptr;
			CheckUI();
			TurnMainButtonUI(true);
		}
		break;
	case ReviewMode:
		m_pGameObject->GetComponent<WeaponQuest>()->CloseReviewUI();
		m_ControlMode = CharacterMode;
		TurnMainButtonUI(true);
		break;
	default:
		break;
	}
}

void Controller::SpaceBarDowned()
{
	if (!(m_Input->GetCurrentKeyState(VirtualKey::SpaceBar) == KeyState::Down)) return;
	switch (m_ControlMode)
	{
	case CharacterMode:

		if (!m_CharacterMovement->m_pGameObject->GetComponent<BoxCollider>()->m_InterActionObjList.empty())
		{
			SoundManager::GetInstance()->SFXPlay("QuestPaper_Open", 3);
			m_CharacterMovement->m_pGameObject->GetComponent<BoxCollider>()->GetIAObj()->GetComponent<Interactable>()->Interact();
			m_CharacterMovement->m_pGameObject->GetComponent<AnimState>()->HandOverStart();

		}
		else
		{
			MakeBubble();
		}

		break;
	case UIMode:
		break;
	case MiniGameMode:
		if (m_Select)
		{
			if (m_Select->Confirm())
			{
				m_UI = nullptr;
				CheckUI();
				m_ControlMode = 0;
				m_pGameObject->GetComponent<WeaponQuest>()->SetCurrentSequence(m_pGameObject->GetComponent<WeaponQuest>()->GetCurrentSequence() + 1);
				TurnMainButtonUI(true);
				SceneManager::GetInstance()->m_HimeEngine->SetFireEffect(true);
			}
		}
		if (m_Hit)
		{
			if (!m_Hit->Confirm())
			{
				m_Hit->Hit();
			}
			else
			{
				m_UI = nullptr;
				CheckUI();
				m_ControlMode = 0;
				m_pGameObject->GetComponent<WeaponQuest>()->SetCurrentSequence(m_pGameObject->GetComponent<WeaponQuest>()->GetCurrentSequence() + 1);
				TurnMainButtonUI(true);
				SceneManager::GetInstance()->m_HimeEngine->SetFireEffect(true);

			}


		}

		break;
	default:
		break;
	}

}

void Controller::SpaceBarPressed()
{
	if (IsPressed(VirtualKey::SpaceBar))
	{
		switch (m_ControlMode)
		{
		case CharacterMode:
			break;
		case UIMode:
			break;
		case MiniGameMode:
			if (m_Hit)
			{
				if (m_UI->m_pGameObject->GetName() == L"MeltingUI")
				{
					m_Hit->Hit();
				}

			}
			break;
		default:
			break;
		}

	}
}

void Controller::MovePressed()
{

	if ((IsPressed(VirtualKey::UpArrow) || IsPressed(VirtualKey::DownArrow) || IsPressed(VirtualKey::LeftArrow) || IsPressed(VirtualKey::RightArrow)))
	{
		switch (m_ControlMode)
		{
		case CharacterMode:
			m_CharacterMovement->RotateObject(DecideDirection());
			m_CharacterMovement->MoveObject();
			m_CharacterMovement->m_pGameObject->GetComponent<AnimState>()->WalkStart();
			break;
		case UIMode:
			break;
		case MiniGameMode:
			break;
		default:
			break;
		}

	}

}

void Controller::MoveDown()
{
	if ((m_Input->GetCurrentKeyState(VirtualKey::UpArrow) == KeyState::Down) || (m_Input->GetCurrentKeyState(VirtualKey::DownArrow) == KeyState::Down) || (m_Input->GetCurrentKeyState(VirtualKey::LeftArrow) == KeyState::Down) || (m_Input->GetCurrentKeyState(VirtualKey::RightArrow) == KeyState::Down))
	{
		switch (m_ControlMode)
		{
		case CharacterMode:
			if (m_ControlMode == CharacterMode)
			{
				m_CharacterMovement->m_pGameObject->GetComponent<AnimState>()->WalkStart();
			}
			break;
		case UIMode:
			if (m_Page)
			{
				SoundManager::GetInstance()->SFXPlay("Menu_Move", 0);
				m_Page->Change(DecideDirection());
			}
			break;
		case MiniGameMode:
			if (m_Page)
			{
				m_Page->Change(DecideDirection());
			}
			break;
		default:
			break;
		}
	}

}

void Controller::TestKeyDown()
{
	if (m_Input->GetCurrentKeyState(VirtualKey::Key_V) == KeyState::Down)
	{
		switch (m_ControlMode)
		{
		case CharacterMode:
			m_TestingUI->OpenUI();
			m_UI = m_TestingUI;
			CheckUI();
			m_ControlMode = MiniGameMode;

			TurnMainButtonUI(false);
			break;
		case UIMode:
			break;
		case MiniGameMode:
			break;
		default:
			break;
		}
	}
	else if (m_Input->GetCurrentKeyState(VirtualKey::Key_U) == KeyState::Down)
	{
		switch (m_ControlMode)
		{
		case CharacterMode:
			if (m_Count <= 0)
			{
				m_Count = 0;
			}
			else
			{
				m_Count--;
			}
			SetTestUI();
			break;
		case UIMode:
			break;
		case MiniGameMode:
			break;
		default:
			break;
		}
	}
	else if (m_Input->GetCurrentKeyState(VirtualKey::Key_I) == KeyState::Down)
	{
		switch (m_ControlMode)
		{
		case CharacterMode:
			if (m_Count >= 7)
			{
				m_Count = 7;
			}
			else
			{
				m_Count++;
			}
			SetTestUI();
			break;
		case UIMode:
			break;
		case MiniGameMode:
			break;
		default:
			break;
		}
	}

	else if (m_Input->GetCurrentKeyState(VirtualKey::Key_L) == KeyState::Down)
	{
		switch (m_ControlMode)
		{
		case CharacterMode:
			m_pGameObject->GetComponent<WeaponQuest>()->OpenReviewUI();
			m_ControlMode = UIMode;
			TurnMainButtonUI(false);

			break;
		case UIMode:
			m_pGameObject->GetComponent<WeaponQuest>()->CloseReviewUI();
			m_ControlMode = CharacterMode;
			TurnMainButtonUI(true);

			break;
		case MiniGameMode:
			break;
		default:
			break;
		}
	}
}

void Controller::MoveKeyUp()
{
	if (m_Input->GetCurrentKeyState(VirtualKey::RightArrow) == KeyState::Up || m_Input->GetCurrentKeyState(VirtualKey::UpArrow) == KeyState::Up || m_Input->GetCurrentKeyState(VirtualKey::DownArrow) == KeyState::Up || m_Input->GetCurrentKeyState(VirtualKey::LeftArrow) == KeyState::Up)
	{
		if (m_ControlMode == CharacterMode)
		{
			m_CharacterMovement->m_pGameObject->GetComponent<AnimState>()->SetIdle();
		}
	}
}

int Controller::DecideDirection()
{
	int _sum = 0;
	int _dir = 5;
	if (IsPressed(VirtualKey::UpArrow)) { _sum += 3; }
	if (IsPressed(VirtualKey::DownArrow)) { _sum += 5; }
	if (IsPressed(VirtualKey::LeftArrow)) { _sum += 13; }
	if (IsPressed(VirtualKey::RightArrow)) { _sum += 19; }

	switch (_sum)
	{
		//UP
	case 3:
		_dir = UP;
		break;

		//DOWN
	case 5:
		_dir = DOWN;
		break;

		//LEFT
	case 13:
		_dir = LEFT;
		break;

		//RIGHT
	case 19:
		_dir = RIGHT;
		break;

		//LEFTUP
	case 16:
		_dir = 7;
		break;

		//RIGHTUP
	case 22:
		_dir = 9;
		break;

		//LEFTDOWN
	case 18:
		_dir = 1;
		break;

		//RIGHT DOWN
	case 24:
		_dir = 3;
		break;

	default:
		_dir = 5;
		break;
	}
	return _dir;

}

void Controller::CheckUI()
{
	if (m_UI == nullptr)
	{
		m_Page = nullptr;
		m_Hit = nullptr;
		m_Select = nullptr;
	}
	else
	{
		if (m_UI->m_pGameObject->GetComponent<UIPage>())
		{
			m_Page = m_UI->m_pGameObject->GetComponent<UIPage>();
			m_Page->UpdateCategory();
			m_Page->UpdatePage();
		}
		if (m_UI->m_pGameObject->GetComponent<SelectMinigame>())
		{
			m_Select = m_UI->m_pGameObject->GetComponent<SelectMinigame>();
			m_Select->SelectUISetUP();

		}
		if (m_UI->m_pGameObject->GetComponent<HitMinigame>())
		{
			m_Hit = m_UI->m_pGameObject->GetComponent<HitMinigame>();
			m_Hit->SetUI();


		}
	}

}

void Controller::MakeBubble()
{
	wstring _name = L"";
	switch (m_pGameObject->GetComponent<WeaponQuest>()->GetCurrentSequence())
	{
	case 0:
		_name = L"Case_bubble";
		break;
	case 1:
		_name = L"Case_bubble";
		break;
	case 2:
		_name = L"Fire_bubble";
		break;
	case 3:
		_name = L"Anvil_bubble";
		break;
	case 4:
		_name = L"Frame_bubble";
		break;
	case 5:
		_name = L"Enchant_bubble";
		break;
	case 6:
		_name = L"Enchant_bubble";
		break;
	case 7:
		_name = L"Counter_bubble";
		break;
	case 8:
		_name = L"Counter_bubble";
		break;
	case 9:
		_name = L"Counter_bubble";
		break;
	default:
		break;
	}

	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetName() == _name)
		{
			obj->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = true;
		}
	}
}
bool Controller::IsPressed(VirtualKey key)
{
	if (!m_Input) return false;
	bool Ishold = KeyState::Hold == m_Input->GetCurrentKeyState(key) ? true : false;
	bool IsDown = KeyState::Down == m_Input->GetCurrentKeyState(key) ? true : false;
	if (Ishold || IsDown)
	{
		return true;
	}
	return false;
}

