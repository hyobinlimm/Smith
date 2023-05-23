#pragma once

#include <windows.h>
#include <HimeRendererStructs.h>

#pragma comment(lib, "HimeEngine_Debug_x64.lib")

class HimeEngine;
class GameTimer;

/// <summary>
/// ������ �����ϴ� �� �����ΰ�
/// 
/// ���� ���� (�÷��̾� ĳ���� Ŭ����, ����, ���)�� �ٷ��.
/// </summary>
class GameProcess
{
public:
	GameProcess();
	~GameProcess();

public:
	HRESULT Initialize(HINSTANCE hInstance);
	void Loop();
	void UpdateAll();
	void RenderAll();
	void Finalize();

private:
	void FPS();

	void CreateObjects();
	
	bool Frame();

private:
	// window Screen
	int m_ScreenX;
	int m_ScreenY;
	int m_ScreenWidth;
	int m_ScreenHeight;

	HWND m_MainHwnd;
	MSG m_Msg;

	POINT mLastMousePos;

private: /// windowAPI �ʱ�ȭ �� �޼��� �Լ�
	HRESULT CreateWindowsHandle(HINSTANCE hInstance);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private: /// ���⼭���ʹ� Game ������ �ʿ��� �͵�
	HimeEngine* m_pD3DEngine;
	static HimeEngine* m_pD3EGinstance;

	GameTimer* m_pTimer;

	TextData* _newText;
	
	std::vector<UIData*> m_texList;
};

