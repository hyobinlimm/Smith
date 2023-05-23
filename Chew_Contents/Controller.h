#pragma once

#include "VirtualKey.h"



class GameObject;
class InputManager;
class SceneManager;

struct TextData;
class Movement;
class UI;
class SelectMinigame;
class HitMinigame;
class UIPage;
/// <summary>
/// 인풋 매니저로부터 인풋을 받아와 키에 할당된 행동을 지시하는 컴포넌트
/// </summary>


class Controller : public Component
{
public:

	void Start();

	void TurnMainButtonUI(bool val);

	void Update();
	void Awake();

	void SetDebugInfo();

	void SetController();

	void SetMode(int mode);

	void SetTestUI();


	void KeyUpdate();
	void SetUI(shared_ptr<UI> ui);
	void OpenQuestUI();
	void OpenMonsterBookUI();
	void CheckUI();

private:

	void ZKeyPressed();
	void XKeyPressed();
	void CKeyPressed();
	void SpaceBarDowned();
	void SpaceBarPressed();
	void MovePressed();
	void MoveDown();
	void TestKeyDown();
	void MoveKeyUp();
	int DecideDirection();
	void MakeBubble();

private:


	shared_ptr<SceneManager> m_Scene;


	//현재 조종하는 대상 0:character 1:UI 2:MiniGame
	int m_ControlMode = 0;

	shared_ptr<Movement> m_CharacterMovement;

	shared_ptr<TextData> m_DebugInfo;
	shared_ptr<TextData> m_DebugInfo2;

private:
	enum eControlMode
	{
		CharacterMode,
		UIMode,
		MiniGameMode,
		ReviewMode
	};

	/*	인풋 관련	*/
	//인풋매니져
	shared_ptr<InputManager> m_Input;
	shared_ptr<UI> m_QuestUI;
	shared_ptr<UI> m_MonsterUI;
	shared_ptr<UI> m_UI;
	shared_ptr<UI> m_MainUI;
	shared_ptr<UI> m_TestingUI;
	shared_ptr<SelectMinigame> m_Select;
	shared_ptr<HitMinigame> m_Hit;
	shared_ptr<UIPage> m_Page;
	//눌렸는지 안눌렸는지 확인하는 기능, down, hold 둘다 포함
	bool IsPressed(VirtualKey key);

	int m_Count = 0;

};

