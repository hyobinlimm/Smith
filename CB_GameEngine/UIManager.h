#pragma once

#include "HimeRendererStructs.h"
#include "UIDataTable.h"

//class UIDataTable;

class UIManager
{
public:
	UIManager() = default;

	~UIManager() = default;

	UIManager(const UIManager& other) = delete;

	UIManager& operator= (const UIManager& other) = delete;

	UIManager(UIManager&& other) = delete;

public:
	static shared_ptr<UIManager> GetInstance();

	void Initialize();
	int GetUICount() { return m_UIData.get()->m_DataCount; }

private:
	static shared_ptr<UIManager> g_UIInstance;

	shared_ptr<UIDataTable> m_UIData;

public:
	vector<shared_ptr<UIData>> m_MainUI;
	vector<shared_ptr<UIData>> m_QuestUI;
	vector<shared_ptr<UIData>> m_MonsterUI;
	vector<shared_ptr<UIData>> m_ReviewUI;	
	vector<shared_ptr<UIData>> m_WeaponUI;
	vector<shared_ptr<UIData>> m_MaterialUI;
	vector<shared_ptr<UIData>> m_MeltingUI;
	vector<shared_ptr<UIData>> m_PouringUI;
	vector<shared_ptr<UIData>> m_HammeringUI;
	vector<shared_ptr<UIData>> m_EnchantSelectUI;
	vector<shared_ptr<UIData>> m_EnchantHitUI;
	vector<shared_ptr<UIData>> m_HandleUI;
	vector<shared_ptr<UIData>> m_TestUI;
	vector<shared_ptr<UIData>> m_CountDownUI;
	vector<shared_ptr<UIData>> m_TitleUI;
	vector<shared_ptr<UIData>> m_CreditUI;
	vector<shared_ptr<UIData>> m_EndingUI;

};

