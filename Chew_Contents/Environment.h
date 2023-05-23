#pragma once
#include <Component.h>


class UIRenderer;
class WeaponQuest;
struct TextData;
class Environment :
	public Component
{


public:
	virtual void Awake() override;


	virtual void Start() override;

	virtual void Update() override;




	int GetFame() const { return m_Fame; }
	void SetFame(int val);
public:


	void CountDay();

	void SetEndingUI();
	void CloseEndingUI();

	void SetDayUI();

	void SetDaySceneUI();

	void SetFameUI();
	void SetClockUI();


private:
	bool IsQuestProcessing();
	int CalculateFame();

	void SetDebugInfo();
	void UpdateDebugInfo();


private:
	shared_ptr<TextData> m_DebugInfo;
	vector<shared_ptr<UIRenderer>> m_UIList;
	vector<shared_ptr<UIRenderer>> m_EndingUIList;
	shared_ptr<WeaponQuest> m_Quest;

	float m_TimePerDay = 420;
	int m_Fame = 8;
	float m_Time = 0;
	int m_Date = 1;
	bool m_IsUITimer = false;
	float m_UITimer = 0;

	bool m_GameEND = false;

};

