#include "CBpch.h"
#include "QuestManager.h"
#include "SceneManager.h"

shared_ptr<QuestManager> QuestManager::g_QuestInstance = nullptr;

std::shared_ptr<QuestManager> QuestManager::GetInstance()
{
	if (g_QuestInstance == nullptr)
		g_QuestInstance = make_shared<QuestManager>();

	return g_QuestInstance;
}

void QuestManager::Initialize()
{

	m_QuestDataTable = make_shared<QuestDataTable>();
	m_QuestDataTable->DeserializeFromFile("../Resource/DataTable/Smith_Quest_Data.json");
}

std::shared_ptr<QuestData> QuestManager::GetQuest()
{
	// ��¥�� ���� 1������ 0~4��, 2������ 5~9��, 3������ 10~14�� ����Ʈ �� �������� ���������� ����.
	std::random_device rd;
	std::mt19937 gen(rd());
	m_QuestDate = SceneManager::GetInstance()->m_CurScene->m_SceneDate;
	shared_ptr<QuestData> _QData = nullptr;
	if (m_QuestDate == 1)
	{

			uniform_int_distribution<int> dis(0, m_QuestDataTable->m_Quest1List.size() - 1);
			int ranNum = dis(gen);
			_QData = m_QuestDataTable->m_Quest1List[ranNum];
			m_QuestDataTable->m_Quest1List.erase(m_QuestDataTable->m_Quest1List.begin() + ranNum);
		
	}
	else if (m_QuestDate == 2)
	{
	
			uniform_int_distribution<int> dis(0, m_QuestDataTable->m_Quest2List.size() - 1);
			int ranNum = dis(gen);
			_QData = m_QuestDataTable->m_Quest2List[ranNum];
			m_QuestDataTable->m_Quest2List.erase(m_QuestDataTable->m_Quest2List.begin() + ranNum);
		

	}
	else
	{
	
			uniform_int_distribution<int> dis(0, m_QuestDataTable->m_Quest3List.size() - 1);
			int ranNum = dis(gen);
			_QData = m_QuestDataTable->m_Quest3List[ranNum];
			m_QuestDataTable->m_Quest3List.erase(m_QuestDataTable->m_Quest3List.begin() + ranNum);
		

	}

		return _QData;
	

}
