#include "resource.h"
#include "CCPch.h"
#include "ClientSystem.h"
#include "CB_GameEngine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TestScene.h"
#include "InputManager.h"
#include "GameTimer.h"
#include "GameScene.h"
#include "TitleScene.h"

shared_ptr<CB_GameEngine> ClientSystem::m_GameEngineInstance = nullptr;
bool ClientSystem::m_Resizing = false;
bool ClientSystem::m_SizeChange = false;

ClientSystem::ClientSystem()
{
	m_GameEngine = make_shared<CB_GameEngine>();
	m_GameEngineInstance = m_GameEngine;
}

ClientSystem::ClientSystem(const ClientSystem& other)
{

}

ClientSystem::~ClientSystem()
{

}

bool ClientSystem::Initialize()
{
	InitializeWindows(m_ScreenWidth, m_ScreenHeight);

	SceneManager::GetInstance()->Initialize();

	SceneManager::GetInstance()->AddGameScene<TestScene>(L"TestScene");
	SceneManager::GetInstance()->AddGameScene<GameScene>(L"GameScene");
	SceneManager::GetInstance()->AddGameScene<TitleScene>(L"TitleScene");
	SceneManager::GetInstance()->LoadScene(L"TitleScene");

	m_GameEngine->Init(m_hwnd, m_ScreenWidth, m_ScreenHeight);
	for (auto& text : SceneManager::GetInstance()->m_CurScene->m_stringList)
	{
		if (text->IDName == L"Screen")
		{
			text->value = m_ScreenWidth;
			text->value2 = m_ScreenHeight;
		}
	}
	SceneManager::GetInstance()->Awake();
	SceneManager::GetInstance()->Start();

	return true;
}


void ClientSystem::Shutdown()
{
	ShutdownWindows();
}

void ClientSystem::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame())
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
			m_GameEngine->Update();
			m_GameEngine->Render();
		}
	}
}


bool ClientSystem::Frame()
{
	return true;
}

void ClientSystem::InitializeWindows(const int& screenWidth, const int& screenHeight)
{
	// 외부 포인터를 이 객체로 지정합니다
	ApplicationHandle = this;

	// 이 프로그램의 인스턴스를 가져옵니다
	m_hinstance = GetModuleHandle(NULL);

	// 프로그램 이름을 지정합니다
	m_applicationName = L"Smith's BlackSmith";

	// windows 클래스를 아래와 같이 설정합니다.
	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;/* | CS_OWNDC;*/
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(m_hinstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;


	// windows class를 등록합니다
	RegisterClassExW(&wc);

	// 모니터 화면의 해상도를 읽어옵니다
	//screenWidth = GetSystemMetrics(SM_CXSCREEN);
	//screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	// FULL_SCREEN 변수 값에 따라 화면을 설정합니다.
	if (FULL_SCREEN)
	{
		// 풀스크린 모드로 지정했다면 모니터 화면 해상도를 데스크톱 해상도로 지정하고 색상을 32bit로 지정합니다.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린으로 디스플레이 설정을 변경합니다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// 윈도우 모드의 경우 800 * 600 크기를 지정합니다.
		//screenWidth = 1920;
		//screenHeight = 1200;

		// 윈도우 창을 가로, 세로의 정 가운데 오도록 합니다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 윈도우를 생성하고 핸들을 구합니다.
	//m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
	//	WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
	//	posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);



	m_hwnd = CreateWindowW(m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW,
		0, 0, m_ScreenWidth, m_ScreenHeight,
		nullptr, nullptr, m_hinstance, NULL);

	// 윈도우를 화면에 표시하고 포커스를 지정합니다
	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(m_hwnd);

	RecalcWindowSize();

	//SetForegroundWindow(m_hwnd);
	//SetFocus(m_hwnd);
}

void ClientSystem::ShutdownWindows()
{
	// 풀스크린 모드였다면 디스플레이 설정을 초기화합니다.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거합니다
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 프로그램 인스턴스를 제거합니다
	UnregisterClassW(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 외부포인터 참조를 초기화합니다
	ApplicationHandle = NULL;
}


void ClientSystem::RecalcWindowSize()
{
	RECT nowRect;
	DWORD _style = (DWORD)GetWindowLong(m_hwnd, GWL_STYLE);
	DWORD _exstyle = (DWORD)GetWindowLong(m_hwnd, GWL_EXSTYLE);

	GetWindowRect(m_hwnd, &nowRect);

	RECT newRect;
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = m_ScreenWidth;
	newRect.bottom = m_ScreenHeight;

	AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);

	// 클라이언트 영역보다 윈도 크기는 더 커야 한다. (외곽선, 타이틀 등)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(m_hwnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

LRESULT CALLBACK ClientSystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//윈도우 종료 확인
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	//윈도우 창  닫힌지 확인
	case WM_CLOSE:
	{

		PostQuitMessage(0);
		return 0;
	}


	// 그 외의 모든 메시지들은 기본 메시지 처리로 넘깁니다.
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}


LRESULT CALLBACK ClientSystem::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 윈도우 종료를 확인합니다
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE:
	{
		m_SizeChange = false;
		m_GameEngineInstance->SetScreen(wparam, lparam, m_Resizing, m_SizeChange);
		return 0;
	}
	case WM_ENTERSIZEMOVE:
	{
		m_Resizing = true;
		return 0;
	}
	case WM_EXITSIZEMOVE:
	{
		m_Resizing = false;
		m_SizeChange = true;
		m_GameEngineInstance->SetScreen(wparam, lparam, m_Resizing, m_SizeChange);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		short zDelta = GET_WHEEL_DELTA_WPARAM(wparam);
		InputManager::GetInstance()->SetMouseWheel(zDelta);
		return 0;
	}

	// 윈도우가 닫히는지 확인합니다
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 그 외의 모든 메시지들은 시스템 클래스의 메시지 처리로 넘깁니다.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}