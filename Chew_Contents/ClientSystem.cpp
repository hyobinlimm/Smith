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
	// �ܺ� �����͸� �� ��ü�� �����մϴ�
	ApplicationHandle = this;

	// �� ���α׷��� �ν��Ͻ��� �����ɴϴ�
	m_hinstance = GetModuleHandle(NULL);

	// ���α׷� �̸��� �����մϴ�
	m_applicationName = L"Smith's BlackSmith";

	// windows Ŭ������ �Ʒ��� ���� �����մϴ�.
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


	// windows class�� ����մϴ�
	RegisterClassExW(&wc);

	// ����� ȭ���� �ػ󵵸� �о�ɴϴ�
	//screenWidth = GetSystemMetrics(SM_CXSCREEN);
	//screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	// FULL_SCREEN ���� ���� ���� ȭ���� �����մϴ�.
	if (FULL_SCREEN)
	{
		// Ǯ��ũ�� ���� �����ߴٸ� ����� ȭ�� �ػ󵵸� ����ũ�� �ػ󵵷� �����ϰ� ������ 32bit�� �����մϴ�.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ������ ���÷��� ������ �����մϴ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// ������ ����� ��� 800 * 600 ũ�⸦ �����մϴ�.
		//screenWidth = 1920;
		//screenHeight = 1200;

		// ������ â�� ����, ������ �� ��� ������ �մϴ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// �����츦 �����ϰ� �ڵ��� ���մϴ�.
	//m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
	//	WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
	//	posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);



	m_hwnd = CreateWindowW(m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW,
		0, 0, m_ScreenWidth, m_ScreenHeight,
		nullptr, nullptr, m_hinstance, NULL);

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �����մϴ�
	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(m_hwnd);

	RecalcWindowSize();

	//SetForegroundWindow(m_hwnd);
	//SetFocus(m_hwnd);
}

void ClientSystem::ShutdownWindows()
{
	// Ǯ��ũ�� ��忴�ٸ� ���÷��� ������ �ʱ�ȭ�մϴ�.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����մϴ�
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���α׷� �ν��Ͻ��� �����մϴ�
	UnregisterClassW(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �ܺ������� ������ �ʱ�ȭ�մϴ�
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

	// Ŭ���̾�Ʈ �������� ���� ũ��� �� Ŀ�� �Ѵ�. (�ܰ���, Ÿ��Ʋ ��)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(m_hwnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

LRESULT CALLBACK ClientSystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//������ ���� Ȯ��
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	//������ â  ������ Ȯ��
	case WM_CLOSE:
	{

		PostQuitMessage(0);
		return 0;
	}


	// �� ���� ��� �޽������� �⺻ �޽��� ó���� �ѱ�ϴ�.
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
		// ������ ���Ḧ Ȯ���մϴ�
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

	// �����찡 �������� Ȯ���մϴ�
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �� ���� ��� �޽������� �ý��� Ŭ������ �޽��� ó���� �ѱ�ϴ�.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}