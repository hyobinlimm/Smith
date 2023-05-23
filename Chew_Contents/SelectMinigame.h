#pragma once
#include <Component.h>

class UIPage;
class WeaponQuest;
class UIRenderer;

class SelectMinigame :
	public Component
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;
public:
	void Select();
	void Cancel();
	void SelectUISetUP();
	bool Confirm();
private:
	vector<shared_ptr<UIRenderer>> m_UIList;
	shared_ptr<UIPage> m_UIPage;
	shared_ptr<WeaponQuest> m_Weapon;
	int m_Count = 1;
};

