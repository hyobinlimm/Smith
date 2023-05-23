#pragma once

#include "KeyState.h"
#include "VirtualKey.h"

/// <summary>
/// Ű�Է��� �޴� ��
/// </summary>


// ���ǵ� ����� Ű�� ����
const __int32 VK_COUNT = 256;

class InputManager
{
public:
	InputManager() = default;
	~InputManager() = default;
	InputManager(const InputManager& other) = delete;
	InputManager& operator= (const InputManager& other) = delete;
	InputManager(InputManager&& other) = delete;

public:
	static shared_ptr<InputManager> GetInstance();

	void Initialize(HWND hWnd);

	void Release();

	void Update();

	KeyState GetCurrentKeyState(VirtualKey key);

	Vector2 GetCurrentMousePosition();

	// ���콺 �� �Է�
	void SetMouseWheel(short zDelta);

	// ���콺 �� ��
	short MouseWheeling() { return m_WheelState; }

	bool GetIsInputAble() const { return IsInputAble; }
	void SetIsInputAble(bool val) { IsInputAble = val; }
private:
	static shared_ptr<InputManager> g_InputInstance;

	BYTE _prevKeyboard[VK_COUNT];
	BYTE _currentKeyboard[VK_COUNT];

	Vector2 _prevMousePosition;
	Vector2 _currentMousePosition;

	HWND _hWnd;

	short m_WheelState;

	bool IsInputAble;
public:
	bool m_IsMouseWheeling;
};
