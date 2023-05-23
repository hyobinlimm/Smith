#pragma once

#include <windows.h>
#include <HimeRendererStructs.h>

#pragma comment(lib, "HimeEngine_Debug_x64.lib")

class HimeEngine;
class GameTimer;

/// <summary>
/// 게임을 진행하는 그 무엇인가
/// 
/// 실제 게임 (플레이어 캐릭터 클래스, 몬스터, 배경)을 다룬다.
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

private: /// windowAPI 초기화 및 메세지 함수
	HRESULT CreateWindowsHandle(HINSTANCE hInstance);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private: /// 여기서부터는 Game 로직에 필요한 것들
	HimeEngine* m_pD3DEngine;
	static HimeEngine* m_pD3EGinstance;

	GameTimer* m_pTimer;

	TextData* _newText;
	
	std::vector<UIData*> m_texList;
};

