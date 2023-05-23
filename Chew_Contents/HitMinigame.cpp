#include "CCPch.h"
#include "GameObject.h"
#include "WeaponQuest.h"
#include "UIRenderer.h"
#include "UI.h"
#include "GameTimer.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "HitMinigame.h"
#include "CountDown.h"

void HitMinigame::Awake()
{

}
void HitMinigame::Start()
{
	m_UIList = m_pGameObject->GetComponent<UI>()->GetUIList();
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"PlayerBar")
		{
			m_MovableBar = _UI;
		}
		else if (_UI->GetUIData()->Type == L"RecogAre")
		{
			m_RecogArea = _UI;
		}
	}

	for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (_obj->GetComponent<WeaponQuest>())
		{
			m_Weapon = _obj->GetComponent<WeaponQuest>();
		}
		else if (_obj->GetName() == L"CountDownUI")
		{
			m_CountDownUIList = _obj->GetComponent<UI>()->GetUIList();
		}
	}

	m_TimerOn = false;
	m_IsEnded = true;
	m_Speed = SceneManager::GetInstance()->m_ScreenWidth * 0.5625;
	m_Direction = 1;
}
void HitMinigame::Update()
{

	if (m_ButtonTimer >= 0.05 && m_ButtonTimerOn)
	{
		m_ButtonTimerOn = false;
		m_ButtonTimer = 0;
		SetButtonUI(false);
	}
	else if (m_ButtonTimerOn)
	{
		if (m_IsEnded)
		{
			m_ButtonTimerOn = false;
			m_ButtonTimer = 0;
		}
		else
		{
			m_ButtonTimer += GameTimer::GetInstance()->DeltaTime();
		}
		
	}


	if (m_CountTimer >= 3 && m_TimerOn)
	{	
		m_IsEnded = false;
		m_TimerOn = false;
		InputManager::GetInstance()->SetIsInputAble(true);
	}
	else if (m_TimerOn)
	{
		if (m_CountTimer >= 2.f)
			m_SoundCount = 2;
		else if (m_CountTimer >= 1.f)
			m_SoundCount = 1;

		if (m_CountTimer == 0.f)
		{
			SoundManager::GetInstance()->SFXPlay("Minigame_Countdown", 5);
		}
		else if (m_CountTimer >= 0.95f && m_CountTimer <= 1.05)
		{
			SoundManager::GetInstance()->SFXPlay("Minigame_Countdown", 5);
		}
		else if (m_CountTimer >= 1.95f && m_CountTimer <= 2.05)
		{
			SoundManager::GetInstance()->SFXPlay("Minigame_Countdown", 5);
		}


		m_CountTimer += GameTimer::GetInstance()->DeltaTime();
	}

	if (!InputManager::GetInstance()->GetIsInputAble()) return;
	m_Speed = SceneManager::GetInstance()->m_ScreenWidth * 0.5625 / 100;
	if (!m_IsEnded)
	{
		Move();

		if (m_pGameObject->GetName() == L"MeltingUI")
		{
			bool InRange = m_MovableBar->GetUIData()->MoveX >= m_RecogArea->GetUIData()->MoveX - m_Speed * 5 && m_MovableBar->GetUIData()->MoveX <= m_RecogArea->GetUIData()->MoveX + m_Speed * 5;
			if (InRange)
			{
				m_Success += 0.2;
			}
			else
			{

			}

			if (m_Count >= 600)
			{
				auto WhichMinigame = m_UIList[0]->GetUIData()->UIType.substr(0, 4);
				m_IsEnded = true;
				m_pGameObject->GetComponent<CountDown>()->TurnEndUI();
				m_Weapon->SetDataI((int)m_Success, WhichMinigame);
			}
			m_Direction = -1;
			m_Count++;
		}
	}
}
void HitMinigame::Hit()
{
	float _Width = SceneManager::GetInstance()->m_ScreenWidth;
	//Hammering -> Hamm을 가져옴
	auto WhichMinigame = m_UIList[0]->GetUIData()->UIType.substr(0, 4);
	SetButtonUI(true);
	if (m_pGameObject->GetName() == L"HammeringUI")
	{
		if (m_CountTimer >= 3)
		{
			SoundManager::GetInstance()->SFXPlay("Minigame_Forging", 1);
		}
		if (m_MovableBar->GetUIData()->MoveX >= -5 * m_Speed && m_MovableBar->GetUIData()->MoveX <= 5 * m_Speed)
		{
			m_Success++;

		}
		if (m_Count >= 4)
		{
			m_pGameObject->GetComponent<CountDown>()->TurnEndUI();
			m_Weapon->SetDataI(m_Success, WhichMinigame);
			m_IsEnded = true;
		}
		else
		{
			m_MovableBar->GetUIData()->MoveX = m_MinBar;
			m_Count++;
		}
	}
	else if (m_pGameObject->GetName() == L"EnchantHitUI")
	{
		CountforSuccess();
		if (m_Count >= 2)
		{
			m_pGameObject->GetComponent<CountDown>()->TurnEndUI();
			m_Weapon->SetDataI(m_Success, WhichMinigame);
			m_IsEnded = true;
		}
		else
		{
			m_Count++;
		}
	}
	else if (m_pGameObject->GetName() == L"PouringUI")
	{
		m_IsEnded = true;
		m_Count /= 2;

		//이거 수정 #수정틀범위
		m_Count += (SceneManager::GetInstance()->m_ScreenHeight - (m_MovableBar->GetUIData()->MoveY + SceneManager::GetInstance()->m_ScreenHeight / 10)) / (SceneManager::GetInstance()->m_ScreenHeight / 30);
		m_pGameObject->GetComponent<CountDown>()->TurnEndUI();
		m_Weapon->SetDataI(m_Count, WhichMinigame);

	}
	else if (m_pGameObject->GetName() == L"MeltingUI")
	{
		if (m_CountTimer >= 3)
			SoundManager::GetInstance()->SFXPlay("Minigame_Melting", 4);
		m_Direction = 1;
	}


}



void HitMinigame::SetButtonUI(bool val)
{
	int _data = 0;
	if (val)
	{
		m_ButtonTimer = 0;
		m_ButtonTimerOn = true;
		_data = 1;
	}
	else
	{
		_data = 0;
	}
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"Button" && _UI->GetUIData()->Data == to_wstring(_data))
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (_UI->GetUIData()->Type == L"Button")
		{
			_UI->GetUIData()->isDraw = false;
		}
	}
}

void HitMinigame::Move()
{
	float _Width = SceneManager::GetInstance()->m_ScreenWidth;
	auto WhichMinigame = m_UIList[0]->GetUIData()->UIType.substr(0, 4);
	if (m_pGameObject->GetName() == L"PouringUI")
	{
		if (m_Count >= 40)
		{
			m_IsEnded = true;
			m_Count /= 2;
			//#수정틀범위
			m_Count += (SceneManager::GetInstance()->m_ScreenHeight - (m_MovableBar->GetUIData()->MoveY + SceneManager::GetInstance()->m_ScreenHeight / 10)) / (SceneManager::GetInstance()->m_ScreenHeight / 30);
			m_Weapon->SetDataI(m_Count, WhichMinigame);
		}
		else if (m_MovableBar->GetUIData()->MoveY <= -SceneManager::GetInstance()->m_ScreenHeight / 10)
		{
			m_Count++;
		}
		else
		{
			m_MovableBar->GetUIData()->MoveY -= SceneManager::GetInstance()->m_ScreenHeight * 1 / 200;
		}

	}
	else if (m_pGameObject->GetName() == L"EnchantHitUI")
	{

		if (IsOutBoundary())
		{
			auto WhichMinigame = m_UIList[0]->GetUIData()->UIType.substr(0, 4);
			m_Weapon->SetDataI(m_Success, WhichMinigame);
			m_IsEnded = true;
		}
		m_MovableBar->GetUIData()->MoveX += m_Speed * m_Direction * (0.55 + (m_Success + 1) * 0.2);
	}
	else if (m_pGameObject->GetName() == L"MeltingUI")
	{
		MeltingMove();
	}
	else if (m_pGameObject->GetName() == L"HammeringUI")
	{

		HammeringMove();

	}
	else
	{

		if (IsOutBoundary())
		{
			m_Direction *= -1;
		}
		m_MovableBar->GetUIData()->MoveX += m_Speed * m_Direction * 0.5;
	}
}

void HitMinigame::HammeringMove()
{
	;
	if (IsOutBoundary())
	{
		m_Direction *= -1;
	}
	m_MovableBar->GetUIData()->MoveX += m_Speed * m_Direction * (0.35 + (m_Success + 1) * 0.2);
}

void HitMinigame::MeltingMove()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_int_distribution<int> dis(-1, 1);
	int ranNum = dis(gen);
	if (m_Direction == -1)
	{
		if (m_MovableBar->GetUIData()->MoveX < m_MinBar)
		{
			m_MovableBar->GetUIData()->MoveX = m_MinBar;
		}
		else
		{
			m_MovableBar->GetUIData()->MoveX += 0.35 * m_Speed * m_Direction;
		}

	}
	else
	{
		if (m_MovableBar->GetUIData()->MoveX > m_MaxBar)
		{
			m_MovableBar->GetUIData()->MoveX = m_MaxBar;
		}
		else
		{
			m_MovableBar->GetUIData()->MoveX += 0.35 * m_Speed * m_Direction;
		}

	}
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"RecogAre")
		{

			if (_UI->GetUIData()->MoveX > m_Speed * 40)
			{
				_UI->GetUIData()->MoveX = m_Speed * 40;
			}
			else if (_UI->GetUIData()->MoveX < -m_Speed * 38)
			{
				_UI->GetUIData()->MoveX = -m_Speed * 38;
			}
			else
			{
				_UI->GetUIData()->MoveX += ranNum * m_Speed * 0.75;
			}

		}
	}
}











void HitMinigame::SetUI()
{
	m_TimerOn = true;
	m_pGameObject->GetComponent<CountDown>()->SetIsTimer(true);
	m_CountTimer = 0;
	m_MinBar = -m_Speed * 45;
	m_MaxBar = m_Speed * 45;
	m_MovableBar->GetUIData()->MoveX = m_MinBar;
	SetButtonUI(false);
	for (auto& _UI : m_CountDownUIList)
	{
		_UI->GetUIData()->isDraw = true;
	}

	InputManager::GetInstance()->SetIsInputAble(false);
	if (m_pGameObject->GetName() == L"EnchantHitUI")
	{
		SetEnchantHitUI();
	}

	else if (m_pGameObject->GetName() == L"MeltingUI")
	{
		SetMeltingUI();
	}
	else if (m_pGameObject->GetName() == L"PouringUI")
	{
		SetPouringUI();

	}
	else if (m_pGameObject->GetName() == L"HammeringUI")
	{
		m_Direction = 1;
	}

	auto WhichMinigame = m_UIList[0]->GetUIData()->UIType.substr(0, 4);
	m_Weapon->SetDataI(0, WhichMinigame);


	m_Count = 0;
	m_Success = 0;
}

void HitMinigame::SetPouringUI()
{
	m_MovableBar->GetUIData()->MoveX = 0;
	m_MovableBar->GetUIData()->MoveY = SceneManager::GetInstance()->m_ScreenHeight * 0.8;

	if (m_CountTimer >= 3)
	{
		SoundManager::GetInstance()->SFXPlay("Minigame_Casting", 2);
	}

	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"WeaponFrame")
		{
			_UI->GetUIData()->isDraw = false;
		}
	}

	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"WeaponFrame" && _UI->GetUIData()->Data == m_Weapon->GetData()->m_WeaponType)
		{
			_UI->GetUIData()->isDraw = true;
		}
	}
}

void HitMinigame::SetMeltingUI()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_int_distribution<int> dis(-30, 30);
	int ranNum = dis(gen);
	m_MovableBar->GetUIData()->MoveX = 0;
	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"RecogAre")
		{
			_UI->GetUIData()->MoveX = ranNum * m_Speed;
		}
	}
	m_Direction = -1;
}

void HitMinigame::SetEnchantHitUI()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_int_distribution<int> dis(0, 15);
	int ranNum = dis(gen);
	m_Goal[0] = 10 + ranNum;

	ranNum = dis(gen);
	m_Goal[1] = 40 + ranNum;

	ranNum = dis(gen);
	m_Goal[2] = 65 + ranNum;

	float _Width = SceneManager::GetInstance()->m_ScreenWidth;


	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"ItemSelect1")
		{
			_UI->GetUIData()->isDraw = false;
		}
		else if (_UI->GetUIData()->Type == L"ItemSelect2")
		{
			_UI->GetUIData()->isDraw = false;
		}

	}

	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"ItemSelect1" && _UI->GetUIData()->Data == m_Weapon->GetData()->m_EnchantType)
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (_UI->GetUIData()->Type == L"ItemSelect2" && _UI->GetUIData()->Data == m_Weapon->GetData()->m_WeaponType)
		{
			_UI->GetUIData()->isDraw = true;
		}
	}


	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->Type == L"RecogAre")
		{
			if (_UI->GetUIData()->Data.find(L"001") != wstring::npos)
			{
				_UI->GetUIData()->MoveX = m_Speed * m_Goal[0] - (_Width / 100000 * 21875);
				m_Goal[0] = _UI->GetUIData()->MoveX;
			}
			else if (_UI->GetUIData()->Data.find(L"002") != wstring::npos)
			{
				_UI->GetUIData()->MoveX = m_Speed * m_Goal[1] - (_Width / 100000 * 21875);
				m_Goal[1] = _UI->GetUIData()->MoveX;
			}
			else if (_UI->GetUIData()->Data.find(L"003") != wstring::npos)
			{
				_UI->GetUIData()->MoveX = m_Speed * m_Goal[2] - (_Width / 100000 * 21875);
				m_Goal[2] = _UI->GetUIData()->MoveX;
			}

		}
	}
}


bool HitMinigame::Confirm()
{
	if (m_IsEnded)
	{
		m_pGameObject->GetComponent<UI>()->CloseUI();
		m_pGameObject->GetComponent<CountDown>()->TurnEndUI();
	}

	return m_IsEnded;
}




void HitMinigame::CountforSuccess()
{
	for (int i = 2; i >= m_Success; i--)
	{
		bool IsInRange = m_MovableBar->GetUIData()->MoveX >= m_Goal[i] - 5 * m_Speed && m_MovableBar->GetUIData()->MoveX <= m_Goal[i] + 5 * m_Speed;
		if (IsInRange)
		{
			m_Success++;
		}
	}
}

bool HitMinigame::IsOutBoundary()
{
	return m_MovableBar->GetUIData()->MoveX < m_MinBar || m_MovableBar->GetUIData()->MoveX > m_MaxBar;
}
