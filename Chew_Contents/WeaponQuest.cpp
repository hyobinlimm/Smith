#include "CCPch.h"
#include "QuestManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "WeaponManager.h"
#include "UIRenderer.h"
#include "UI.h"
#include "WeaponQuest.h"
#include "AIController.h"
#include "Environment.h"

void WeaponQuest::Awake()
{
	SetDebugInfo();
}

void WeaponQuest::Start()
{
	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetName() == L"ReviewUI")
		{
			m_UIList = obj->GetComponent<UI>()->GetUIList();
		}
	}
	m_QuestInfo = make_shared<QuestData>();
	m_WeaponData = make_shared<CompareData>();
	m_CurrentSequence = 9;

}

void WeaponQuest::Update()
{
	UpdateDebugInfo();
}


void WeaponQuest::SetNewQuest()
{
	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetComponent<AIController>() != nullptr)
		{
			m_QuestInfo = obj->GetComponent<AIController>()->GetWarriorQuest();
		}
	}
}
void WeaponQuest::OpenReviewUI()
{
	for (auto& _UI : m_UIList)
	{

		_UI->GetUIData()->isDraw = true;

	}



	for (auto& _UI : m_UIList)
	{
		if (_UI->GetUIData()->FileName.find(m_QuestInfo->m_QuestID + L"_" + CalculateSatistification()) != wstring::npos && m_QuestInfo->m_QuestID != L"")
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (_UI->GetUIData()->Type == L"Review_Script")
		{
			_UI->GetUIData()->isDraw = false;
		}


		bool IsCorrectHandle = _UI->GetUIData()->Type == L"WeaponHandle" && _UI->GetUIData()->Data == m_WeaponData->m_Handle;
		bool isBigSize = m_WeaponData->m_WeaponType.find(L"01") != wstring::npos && _UI->GetUIData()->Name.find(L"Big") != wstring::npos;
		bool isSmallSize = m_WeaponData->m_WeaponType.find(L"02") != wstring::npos && _UI->GetUIData()->Name.find(L"Small") != wstring::npos;
		bool isMorningStar = m_WeaponData->m_WeaponType.find(L"302") != wstring::npos && _UI->GetUIData()->Name.find(L"Big") != wstring::npos;

		if (isMorningStar && IsCorrectHandle)
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (IsCorrectHandle && isBigSize)
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (IsCorrectHandle && isSmallSize)
		{
			if (m_WeaponData->m_WeaponType.find(L"302") != wstring::npos)
			{
				_UI->GetUIData()->isDraw = false;
			}
			else
			{
				_UI->GetUIData()->isDraw = true;
			}

		}
		else if (_UI->GetUIData()->Type == L"WeaponHandle")
		{
			_UI->GetUIData()->isDraw = false;
		}


		bool IsCorrectHead = _UI->GetUIData()->Type == L"WeaponHead" && _UI->GetUIData()->Data == m_WeaponData->m_WeaponType;
		bool IsWeaponGood = IsWeaponSuccess() && _UI->GetUIData()->Name.find(L"good") != wstring::npos;
		bool IsWeaponBad = !IsWeaponSuccess() && _UI->GetUIData()->Name.find(L"bad") != wstring::npos;
		if (IsCorrectHead && IsWeaponGood)
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (IsCorrectHead && IsWeaponBad)
		{
			_UI->GetUIData()->isDraw = true;
		}

		else if (_UI->GetUIData()->Type == L"WeaponHead")
		{
			_UI->GetUIData()->isDraw = false;
		}

		if (_UI->GetUIData()->Type == L"Effect" && m_WeaponData->m_EnchantType == _UI->GetUIData()->Data)
		{
			_UI->GetUIData()->isDraw = true;
		}
		else if (_UI->GetUIData()->Type == L"Effect")
		{
			_UI->GetUIData()->isDraw = false;
		}
	}

}

void WeaponQuest::CloseReviewUI()
{
	for (auto& _UI : m_UIList)
	{
		_UI->GetUIData()->isDraw = false;
	}
	m_CurrentSequence = 9;
	m_QuestInfo.reset();
	m_WeaponData.reset();

	m_QuestInfo = make_shared<QuestData>();
	m_WeaponData = make_shared<CompareData>();
	QuestManager::GetInstance()->SetClearedQuestCount(1);
}

void WeaponQuest::SetDebugInfo()
{
	m_DebugInfo = make_shared<TextData>();
	m_DebugInfo->IDName = L"Quest";
	m_DebugInfo->text = L"Current Quest Info :\n %ls";
	m_DebugInfo->color = { 1.f, 1.f, 1.f, 1.f };
	m_DebugInfo->FontSize = 2.f;
	m_DebugInfo->x = 1800;
	m_DebugInfo->y = 360;
	m_DebugInfo->textValue = L"";
	m_DebugInfo->isDraw = false;
	SceneManager::GetInstance()->m_CurScene->m_stringList.emplace_back(m_DebugInfo);

	m_DebugInfo3 = make_shared<TextData>();
	m_DebugInfo3->IDName = L"Weapon";
	m_DebugInfo3->text = L"Current Weapon Info :\n %ls";
	m_DebugInfo3->color = { 1.f, 1.f, 1.f, 1.f };
	m_DebugInfo3->FontSize = 2.f;
	m_DebugInfo3->x = 50;
	m_DebugInfo3->y = 360;
	m_DebugInfo3->textValue = L"";
	m_DebugInfo3->isDraw = false;
	SceneManager::GetInstance()->m_CurScene->m_stringList.emplace_back(m_DebugInfo3);
}

void WeaponQuest::UpdateDebugInfo()
{
	wstring _newWst;
	_newWst =
		+L"Weapon Category : "
		+ m_WeaponData->m_WeaponCategory
		+ L"\n"
		+ L"Weapon Type : "
		+ m_WeaponData->m_WeaponType
		+ L"\n"
		+ L"Weapon Hardness : "
		+ to_wstring(m_WeaponData->m_Hardness)
		+ L"\n"
		+ L"Weapon m_EnchantType : "
		+ m_WeaponData->m_EnchantType
		+ L"\n"
		+ L"Weapon m_EnchantSuccess : "
		+ to_wstring(m_WeaponData->m_EnchantSuccess)
		+ L"\n"
		+ L"Weapon m_Handle : "
		+ m_WeaponData->m_Handle
		+ L"\n"
		+ L"Weapon m_Purity : "
		+ to_wstring(m_WeaponData->m_Purity)
		+ L"\n"
		+ L"Weapon m_Sharpness : "
		+ to_wstring(m_WeaponData->m_Sharpness)
		+ L"\n"
		+ L"Weapon m_Weight : "
		+ to_wstring(m_WeaponData->m_Weight)
		+ L"\n"
		+ L"Weapon m_Mat1 : "
		+ m_WeaponData->m_Mat1
		+ L"\n"
		+ L"Weapon m_Mat2 : "
		+ m_WeaponData->m_Mat2
		+ L"\n";

	m_DebugInfo3->textValue = _newWst;



	_newWst =
		+L"Quest ID : "
		+ m_QuestInfo->m_QuestID
		+ L"\n"
		+ L"Quest Category : "
		+ m_QuestInfo->m_QWeaponCategory
		+ L"\n"
		+ L"Quest Type : "
		+ m_QuestInfo->m_QWeaponType
		+ L"\n"
		+ L"Quest m_EnchantType : "
		+ m_QuestInfo->m_QEnchant
		+ L"\n"
		+ L"Quest m_Handle : "
		+ m_QuestInfo->m_QHandle
		+ L"\n"
		+ L"Quest m_Mat1 : "
		+ m_QuestInfo->m_QMat1
		+ L"\n"
		+ L"Quest m_Mat2 : "
		+ m_QuestInfo->m_QMat2
		+ L"\n";
	m_DebugInfo->textValue = _newWst;
}


void WeaponQuest::SetDataW(wstring amount, wstring which, int sequence)
{
	if (which == L"W")
	{
		m_WeaponData->m_WeaponType = L"WT" + amount;
		m_WeaponData->m_WeaponCategory = L"WC00" + amount.substr(0, 1);
	}
	else if (which == L"M")
	{
		if (sequence == 1)
		{
			m_WeaponData->m_Mat1 = L"M" + amount;


		}
		else
		{
			m_WeaponData->m_Mat2 = L"M" + amount;

			if (m_WeaponData->m_Mat2.find(to_wstring(0)) != wstring::npos)
			{
				m_WeaponData->m_Hardness =
					m_WeaponData->m_Hardness = WeaponManager::GetInstance()->GetMatData(m_WeaponData->m_Mat1)->m_MatHardness;
				+WeaponManager::GetInstance()->GetMatData(m_WeaponData->m_Mat2)->m_MatHardness;
			}

		}
	}
	else if (which == L"E")
	{
		m_WeaponData->m_EnchantType = L"E" + amount;
	}
	else if (which == L"H")
	{
		m_WeaponData->m_Handle = L"H" + amount;
	}
}

void WeaponQuest::SetDataI(int amount, wstring which)
{
	if (which == L"Melt")
	{
		m_WeaponData->m_Purity = amount;
	}
	else if (which == L"Pour")
	{
		m_WeaponData->m_Weight = amount;
	}
	else if (which == L"Hamm")
	{
		m_WeaponData->m_Sharpness = amount;
	}
	else if (which == L"Ench")
	{
		m_WeaponData->m_EnchantSuccess = amount;
	}
}


bool WeaponQuest::IsQuestSuccess()
{
	int _questCount = 0;
	bool IsWeaponType = m_WeaponData->m_WeaponType == m_QuestInfo->m_QWeaponType || m_QuestInfo->m_QWeaponType == to_wstring(0);
	bool IsWeaponCategory = m_WeaponData->m_WeaponCategory == m_QuestInfo->m_QWeaponCategory || m_QuestInfo->m_QWeaponCategory == to_wstring(0);
	bool IsHardnessMax = m_WeaponData->m_Hardness <= m_QuestInfo->m_QHardnessMax;
	bool IsHardnessMin = m_WeaponData->m_Hardness >= m_QuestInfo->m_QHardnessMin;
	bool IsEnchantType = m_WeaponData->m_EnchantType == m_QuestInfo->m_QEnchant || m_QuestInfo->m_QEnchant == to_wstring(0);
	bool IsHandleType = m_WeaponData->m_Handle == m_QuestInfo->m_QHandle || m_QuestInfo->m_QHandle == to_wstring(0);

	if (IsWeaponCategory)
	{
		_questCount++;
	}

	if (IsWeaponType)
	{
		_questCount++;
	}

	if (IsMaterialSuccess())
	{
		if (IsHardnessMin)
		{
			if (IsHardnessMax)
			{
				_questCount++;
			}
			else if (m_QuestInfo->m_QHardnessMax == 0)
			{
				_questCount++;
			}
		}
	}

	if (IsEnchantType)
	{
		_questCount++;
	}

	if (IsHandleType)
	{
		_questCount++;
	}

	if (_questCount >= 4)
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool WeaponQuest::IsWeaponSuccess()
{
	int _weaponCount = 0;
	bool IsPurity = m_WeaponData->m_Purity >= 80;
	bool IsWeight = m_WeaponData->m_Weight <= 33 && m_WeaponData->m_Weight >= 24;
	bool IsSharpness = m_WeaponData->m_Sharpness >= 4;
	bool IsEnchantSuccess = m_WeaponData->m_EnchantSuccess >= 2;

	if (IsPurity)
	{
		_weaponCount++;
	}


	if (IsWeight)
	{
		_weaponCount++;
	}


	if (IsSharpness)
	{
		_weaponCount++;
	}


	if (IsEnchantSuccess)
	{
		_weaponCount++;
	}

	if (_weaponCount >= 3)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool WeaponQuest::IsMaterialSuccess()
{
	if ((m_QuestInfo->m_QMat1 == to_wstring(0)) && (m_QuestInfo->m_QMat2 == to_wstring(0)))
	{
		return true;
	}
	else if ((m_QuestInfo->m_QMat1 == to_wstring(0)) || (m_QuestInfo->m_QMat2 == to_wstring(0)))
	{
		if (m_QuestInfo->m_QMat1 == to_wstring(0))
		{
			if (m_QuestInfo->m_QMat2 == m_WeaponData->m_Mat1 || m_QuestInfo->m_QMat2 == m_WeaponData->m_Mat2)
			{
				return true;
			}
		}
		else if (m_QuestInfo->m_QMat2 == to_wstring(0))
		{
			if (m_QuestInfo->m_QMat1 == m_WeaponData->m_Mat1 || m_QuestInfo->m_QMat1 == m_WeaponData->m_Mat2)
			{
				return true;
			}
		}
	}
	else
	{
		if (m_QuestInfo->m_QMat1 == m_WeaponData->m_Mat1 && m_QuestInfo->m_QMat2 == m_WeaponData->m_Mat2)
		{
			return true;
		}
		else if (m_QuestInfo->m_QMat1 == m_WeaponData->m_Mat2 && m_QuestInfo->m_QMat2 == m_WeaponData->m_Mat1)
		{
			return true;

		}
	}
	return false;
}

bool WeaponQuest::IsCurrentSequence(wstring name)
{
	int _sequence = 0;
	if (name == L"WeaponUI")
	{
		_sequence = 1;
	}
	else if (name == L"MaterialUI")
	{
		_sequence = 2;
	}
	else if (name == L"MeltingUI")
	{
		_sequence = 3;
	}
	else if (name == L"PouringUI")
	{
		_sequence = 4;
	}
	else if (name == L"HammeringUI")
	{
		_sequence = 5;
	}
	else if (name == L"EnchantSelectUI")
	{
		_sequence = 6;
	}
	else if (name == L"EnchantHitUI")
	{
		_sequence = 7;
	}
	else if (name == L"HandleUI")
	{
		_sequence = 8;
	}

	if (_sequence == GetCurrentSequence())
	{
		return true;
	}
	return false;
}

std::wstring WeaponQuest::GetQuestID()
{
	return m_QuestInfo->m_QuestID;
}

wstring WeaponQuest::CalculateSatistification()
{


	if (IsWeaponSuccess() && IsQuestSuccess())
	{
		for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
		{
			if (_obj->GetName().find(L"Environment") != wstring::npos)
			{
				_obj->GetComponent<Environment>()->SetFame(2);
			}
		}
		return L"GG";
	}
	else if (IsWeaponSuccess() || IsQuestSuccess())
	{
		if (IsWeaponSuccess())
		{
			for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
			{
				if (_obj->GetName().find(L"Environment") != wstring::npos)
				{
					_obj->GetComponent<Environment>()->SetFame(1);
				}
			}
			return L"GB";
		}
		else if (IsQuestSuccess())
		{
			for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
			{
				if (_obj->GetName().find(L"Environment") != wstring::npos)
				{
					_obj->GetComponent<Environment>()->SetFame(1);
				}
			}
			return L"BG";
		}
	}
	else
	{
		for (auto& _obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
		{
			if (_obj->GetName().find(L"Environment") != wstring::npos)
			{
				_obj->GetComponent<Environment>()->SetFame(-2);
			}
		}
		return L"BB";
	}
}


