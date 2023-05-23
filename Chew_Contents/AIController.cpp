#include "CCPch.h"
#include "Movement.h"
#include "AIController.h"
#include "MeshRenderer.h"
#include "QuestManager.h"

AIController::AIController()
{

}

void AIController::Awake()
{

}

void AIController::Start()
{

}

void AIController::Update()
{

}

void AIController::MakeWarrior()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_int_distribution<int> dis(0, 3);
	int ranNum = dis(gen);

	if (!m_WarriorList.empty())
	{
		m_WarriorList[ranNum]->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = true;
		m_QuestInfo = QuestManager::GetInstance()->GetQuest();
	}
}

void AIController::DeleteWarrior()
{
	for (auto& obj : m_WarriorList)
	{
		obj->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = false;
	}
}
