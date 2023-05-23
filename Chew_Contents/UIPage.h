#pragma once
#include <Component.h>


class UIRenderer;

class UIPage :
	public Component
{
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
public:
	void Change(int direction);
	void UpdatePage();
	void UpdateCategory();

	int GetIndex() { return m_Index; };
	int GetCategory() { return m_Category; };
private:
	vector<shared_ptr<UIRenderer>> m_UIList;
	int m_Index = 1;
	int m_Category = 1;
	int m_MaxCategory = 0;
	int m_MaxIndex = 0;
};

