#pragma once
#include <Component.h>

class UIRenderer;
class SceneManager;
class UIController :
    public Component
{
public:
    void Awake();
    void Start();
    void Update();

    void OpenUI();
    void CloseUI();

    void UpdatePage();

    wstring CheckIndex();
    wstring CheckCategory();
    void Move(int dir);

    void SetUI(int type);
    int GetUI();


public:
    enum eUIType
    {
        QUEST= 1,
        BOOK,
        Mini1
    };

private:
    int m_Type=0;
    vector<shared_ptr<UIRenderer>> m_QuestUI;
    vector<shared_ptr<UIRenderer>> m_BookUI;
	vector<shared_ptr<UIRenderer>> m_ButtonUI;
	vector<shared_ptr<UIRenderer>> m_Mini1UI;
    shared_ptr<SceneManager> m_Scene; 
    int m_Index = 1;
    int m_Category = 1;
    bool m_IsUIOpen;

};

