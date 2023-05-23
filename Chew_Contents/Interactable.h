#pragma once

class Controller;
class UI;
class WeaponQuest;
class AIController;

class Interactable :
    public Component
{
public:
	virtual void Awake() override;


	virtual void Start() override;


	virtual void Update() override;

	void SetUI();
	void Interact();

	void OpenMiniGame();

	int GetSequence() const { return m_Sequence; }
	void SetSequence(int val) { m_Sequence = val; }
private:
	int m_Sequence;
	shared_ptr<Controller> m_PlayerController;
	shared_ptr<UI> m_UI;
	shared_ptr<WeaponQuest> m_Weapon;
	shared_ptr<AIController> m_Warrior;
};

