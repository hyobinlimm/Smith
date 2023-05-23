#pragma once
#include <Component.h>
#include "WeaponQuest.h"



class UIRenderer;
struct CompareData;
class QuestUI :
    public Component
{
public:
	virtual void Awake() override;


	virtual void Start() override;


	virtual void Update() override;

	void QuestUISetup();
private:
	vector<shared_ptr<UIRenderer>> m_UIList;
	shared_ptr<WeaponQuest> m_Quest;

};

