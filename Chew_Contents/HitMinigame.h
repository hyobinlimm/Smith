#pragma once
#include <Component.h>


class UIRenderer;
class HitMinigame :
	public Component
{
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
public:



	void Hit();


	//ButtonUI
	void SetButtonUI(bool val);

	//UI 이동
	void Move();
	void HammeringMove();
	void MeltingMove();


	//UI Open할시 초기 세팅
	void SetUI();
	void SetPouringUI();
	void SetMeltingUI();
	void SetEnchantHitUI();

	//게임이 완료됬나 안됬나 체크
	bool Confirm();

	//인챈트 성공 체크
	void CountforSuccess();


private:
	bool IsOutBoundary();
	float m_Speed = 1.0f;
	int m_Direction = 1;
	bool m_IsParallel;
	bool m_IsEnded = true;
	vector<shared_ptr<UIRenderer>> m_UIList;
	vector<shared_ptr<UIRenderer>> m_CountDownUIList;
	shared_ptr<UIRenderer> m_MovableBar;
	shared_ptr<UIRenderer> m_RecogArea;
	shared_ptr<WeaponQuest> m_Weapon;
	int m_Count = 0;
	float m_Success = 0;
	int m_Goal[3] = { 0 };
	bool m_TimerOn;
	bool m_ButtonTimerOn;
	float m_CountTimer = 0;
	float m_ButtonTimer = 0;
	int m_MinBar=0;
	int m_MaxBar=0;
	int m_SoundCount = 0;
};

