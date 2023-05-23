#pragma once
#include <Component.h>

struct QuestData;

class AIController : public Component
{
public:
	AIController();
	virtual ~AIController() {};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

public:
	shared_ptr<QuestData> GetWarriorQuest() { return m_QuestInfo; }
	void MakeWarrior();
	void DeleteWarrior();

private:
	shared_ptr<QuestData> m_QuestInfo;

public:
	vector<shared_ptr<GameObject>> m_WarriorList;
};

