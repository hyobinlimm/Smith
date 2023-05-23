#pragma once
#include <Component.h>


class UIRenderer;
class CountDown :
	public Component
{
public:
	virtual void Awake() override;


	virtual void Start() override;


	virtual void Update() override;

	void TurnEndUI();

	bool GetIsTimer() const { return m_IsTimer; }
	void SetIsTimer(bool val);
public:

	void UpdateCountdownTimerUI();

private:

	float m_CountdownTimer = 0;
	float m_PrevTimer = 0;
	bool m_IsTimer = false;

	vector<shared_ptr<UIRenderer>> m_UIList;


};

