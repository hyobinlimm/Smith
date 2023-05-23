#pragma once

#include "QuestDataTable.h"

class QuestManager
{
public:
	QuestManager() = default;

	~QuestManager() = default;

	QuestManager(const QuestManager& other) = delete;

	QuestManager& operator= (const QuestManager& other) = delete;

	QuestManager(QuestManager&& other) = delete;

	int GetClearedQuestCount() const { return m_ClearedQuestCount; }
	void SetClearedQuestCount(int val) { m_ClearedQuestCount += val; }
public:
	static shared_ptr<QuestManager> GetInstance();

	void Initialize();
	shared_ptr<QuestData> GetQuest();


private:
	static shared_ptr<QuestManager> g_QuestInstance;



	shared_ptr<QuestDataTable> m_QuestDataTable;
	int m_QuestDate;

	int m_ClearedQuestCount = 0;
};

