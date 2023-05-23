#pragma once

#include <Component.h>

class SceneManager;
class UIRenderer;
struct UIData;
class WeaponQuest;
class Quest;
struct TextData;

class MiniGame : public Component
{
public:
	enum eGameType
	{
		SELECT = 1,
		HIT,
		KEEP
	};
	enum eWork
	{
		WEAPONSELECT = 1,
		MATERIALSELECT,
		MELTING,
		POURING,
		HAMMERING,
		ENCHANTSELECT,
		ENCHANTHIT,
		HANDGUARDING
	};
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	void SetUI();

	void DrawSelect();
	void DrawSelected(int count);
	void DrawHit();

	void Move(int direction);
	void Action();
	void Cancel();
	void Confirm();
	void Quit();

	void Reset();

private:
	shared_ptr<SceneManager> m_Scene;
	vector<shared_ptr<UIRenderer>> m_CurMini;
	vector<shared_ptr<UIRenderer>> m_WeaponUI;
	vector<shared_ptr<UIRenderer>> m_MaterialUI;
	vector<shared_ptr<UIRenderer>> m_Minigame1UI;
	vector<shared_ptr<UIRenderer>> m_Minigame2UI;
	vector<shared_ptr<UIRenderer>> m_Minigame3UI;
	vector<shared_ptr<UIRenderer>> m_EnchantUI;
	vector<shared_ptr<UIRenderer>> m_Minigame4UI;
	vector<shared_ptr<UIRenderer>> m_HandleUI;

	int m_CurrentWork = 0;
	int m_GameType = 0;
	int m_Category = 1;
	int m_Index = 1;
	int m_Count;
	int m_TryCount;
	int m_IndexMax = 1;
	int m_CategoryMax = 1;
	int m_Selected[2] = { 0 };
	int m_RandCount[3] = { 0 };
	float m_Speed = 2.0f;

	shared_ptr<UIRenderer> m_MoveBar;
	shared_ptr<UIRenderer> m_FishBar;


};

