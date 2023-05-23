#include "CBpch.h"
#include "InputManager.h"


shared_ptr<InputManager> InputManager::g_InputInstance = nullptr;

std::shared_ptr<InputManager> InputManager::GetInstance()
{
	if (g_InputInstance == nullptr)
		g_InputInstance = make_shared<InputManager>();

	return g_InputInstance;
}

void InputManager::Initialize(HWND hWnd)
{
	_hWnd = hWnd;

	ZeroMemory(_prevKeyboard, sizeof(BYTE) * VK_COUNT);

	BOOL success = GetKeyboardState(_currentKeyboard);

	assert(success);

	_prevMousePosition = Vector2(0.f, 0.f);

	_currentMousePosition = Vector2(0.f, 0.f);

	m_IsMouseWheeling = false;

	IsInputAble = true;
}

void InputManager::Release()
{

}

void InputManager::Update()
{
	// 과거 키보드 상태를 초기화합니다.
	memcpy_s(_prevKeyboard, VK_COUNT * sizeof(BYTE),
		_currentKeyboard, VK_COUNT * sizeof(BYTE));

	BOOL success = GetKeyboardState(_currentKeyboard);

	assert(success);

	_prevMousePosition = _currentMousePosition;

	POINT currentMousePos;

	GetCursorPos(&currentMousePos);

	ScreenToClient(_hWnd, &currentMousePos);

	_currentMousePosition = Vector2(static_cast<float>(currentMousePos.x),
		static_cast<float>(currentMousePos.y));
}

KeyState InputManager::GetCurrentKeyState(VirtualKey key)
{
	int checkKey = static_cast<int>(key);

	if ((checkKey < 0) || (checkKey > 255))
		return KeyState::None;

	const bool previousPressed = _prevKeyboard[checkKey] & 0b10000000;
	const bool currentPressed = _currentKeyboard[checkKey] & 0b10000000;

	if (previousPressed == false)
		return !currentPressed ? KeyState::None : KeyState::Down;
	else
		return !currentPressed ? KeyState::Up : KeyState::Hold;
}

DirectX::SimpleMath::Vector2 InputManager::GetCurrentMousePosition()
{
	return _currentMousePosition;
}

void InputManager::SetMouseWheel(short zDelta)
{
	m_WheelState = zDelta;
	
	m_IsMouseWheeling = true;
}
