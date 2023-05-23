// DirtyGameEngine.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//
#include "framework.h"
#include "GameProcess.h"
#include "HimeEngine.h"
#include "GameTimer.h"
#include <tchar.h>
#include "BuildInfo.h"

#define MAX_LOADSTRING 100

HimeEngine* GameProcess::m_pD3EGinstance = nullptr;

GameProcess::GameProcess()
	: m_ScreenX(0), m_ScreenY(0), m_ScreenWidth(1920), m_ScreenHeight(800),
	m_MainHwnd(nullptr), m_Msg(),
	m_pD3DEngine(nullptr),
	mLastMousePos{ 0,0 },
	m_pTimer(nullptr)
{

}

GameProcess::~GameProcess()
{
	delete m_pD3DEngine;
	m_pD3DEngine = nullptr;
}

HRESULT GameProcess::CreateWindowsHandle(HINSTANCE hInstance)
{
	WCHAR szTitle[MAX_LOADSTRING] = L"GameEngineDemo";                  // 제목 표시줄 텍스트입니다.
	WCHAR szWindowClass[MAX_LOADSTRING] = L"today";            // 기본 창 클래스 이름입니다.

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassExW(&wcex);

	m_MainHwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		m_ScreenX, m_ScreenY, m_ScreenWidth, m_ScreenHeight, nullptr, nullptr, hInstance, nullptr);

	if (!m_MainHwnd)
	{
		return -1;
	}

	return S_OK;

	// (int)S_FALSE 는 1 ,(int)S_OK는 0 이더라. 그래서 FAILED()를 했을데 모두 거짓으로 판정.
	// SUCCESED() >= 0 으로 참을 판정
	// FAILED() <  0 으로 참을 판정 
	// 그래서 return -1로 FAILED() 참을 만들려고 함. 
}

HRESULT GameProcess::Initialize(HINSTANCE hInstance)
{
	int hr = CreateWindowsHandle(hInstance);

	if (FAILED(hr))
	{
		MessageBox(0, L"CreateWindowsHandle Failed.", 0, 0);
		return S_FALSE;
	}

	/// 3D 그래픽스 초기화 
	m_pD3DEngine = new HimeEngine();
	m_pD3DEngine->Initialize(m_MainHwnd, m_ScreenWidth, m_ScreenHeight);

	m_pD3EGinstance = m_pD3DEngine;

	CreateObjects(); // 만들 오브젝트 요청 

	//----------------------------------------------------------------------
	// 빌드 정보와 객체를 생성해주자.
	FilePath* _pFilePath = new FilePath;
	//_pFilePath->ID_name = L"pig";
	//_pFilePath->TexName = L"RainbowTree_Texture01";
	//_pFilePath->FileName = L"babypig_walk_6x";

	//_pFilePath->ID_name = L"genji";
	//_pFilePath->TexName = L"000000002405";
	//_pFilePath->FileName = L"genji_max";

	_pFilePath->ID_name = L"box";
	_pFilePath->TexName = L"WoodCrate02";
	_pFilePath->FileName = L"box";

	//_pFilePath->ID_name = L"Cylinder";
	//_pFilePath->TexName = L"000000002405";
	//_pFilePath->FileName = L"Cylinder";

	//_pFilePath->ID_name = L"03IK-Joe";
	//_pFilePath->TexName = L"000000002405";
	//_pFilePath->FileName = L"03IK-Joe_Idle";		/// Idle애니메이션 이름 적용한 조
	///_pFilePath->FileName = L"03IK-Joe_onlymesh";

	//_pFilePath->ID_name = L"Test";
	//_pFilePath->TexName = L"warrior_d_texture";
	//////_pFilePath->TexName = L"000000002405";
	//_pFilePath->FileName = L"character_warrior_D";

	//_pFilePath->ID_name = L"test";
	//_pFilePath->TexName = L"warrior_a_texture";
	//_pFilePath->FileName = L"character_warrior_A";

	std::vector<FilePath*> _filePath;
	_filePath.push_back(_pFilePath);

	m_pD3DEngine->SetBuildInfoAll(_filePath);
	//----------------------------------------------------------------------

	// text 출력 예시 코드 
	//char strID[] = "치우를 위한 테스트 코드, %d %d";
	//m_pD3DEngine->CreateStringBuffer(200, 400, 2.0f, { 1.f, 0.f, 1.f, 1.f }, strID);

	XMFLOAT4 _color{ 1.f, 1.f, 0.f, 1.f };

	/*_newText = new TextData();
	_newText->IDName = L"text1";
	_newText->x = 500;
	_newText->y = 500;
	_newText->FontSize = 1.f;
	_newText->color = _color;
	_newText->text = _T("치우를 위한 테스트 코드,");

	m_pD3DEngine->CreateStringBuffer(_newText);
*/
	/*UIData* _texture1 = new UIData;
	_texture1->UIName = L"test";
	_texture1->FileName = L"test";
	_texture1->left = 100;
	_texture1->top = 100;
	_texture1->right = 400;
	_texture1->bottom = 400;
	_texture1->isDraw = true;*/

	/*TextureData* _texture2 = new TextureData;
	_texture2->name = "WoodCrate01";
	_texture2->left = 400;
	_texture2->top = 400;
	_texture2->right = 800;
	_texture2->bottom = 800;*/

	//m_texList.push_back(_texture1);
	//_texList.push_back(_texture2);

	//m_pD3DEngine->SetUITextureData(m_texList);

	m_pTimer = new GameTimer();
	m_pTimer->Reset();

	ShowWindow(m_MainHwnd, SW_SHOWNORMAL);
	UpdateWindow(m_MainHwnd);

	return S_OK;
}

void GameProcess::Loop()
{
	while (TRUE)
	{
		if (PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_Msg.message == WM_QUIT) break;
			DispatchMessage(&m_Msg);
		}
		else
		{
			if (!Frame())
			{
				MessageBox(m_MainHwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}

			UpdateAll();
			RenderAll();
		}
	}
}

void GameProcess::UpdateAll()
{
	// 매 프레임 시간을 계산
	m_pTimer->Tick();

	// 매 프레임의 deltaTime
	m_pTimer->DeltaTime();

	FPS(); // todo:: 60프레임 제한을 걸어줘야 한다. 


	//----------------------------------------------------------------------
	// UI 출력 예시 코드 
	
	//----------------------------------------------------------------------

	m_pD3DEngine->Update(m_pTimer->DeltaTime());

	/*_newText->x += 1;

	m_pD3DEngine->TextUpdate(_newText);*/

	/*_texList.at(1)->left += 1;
	_texList.at(1)->right += 1;

	m_pD3DEngine->UIUpdate(_texList.at(1));*/
}

void GameProcess::RenderAll()
{
	m_pD3DEngine->Render();
}

void GameProcess::Finalize()
{
	m_pD3DEngine->Destroy();
}

void GameProcess::FPS()
{
	/// 타이머를 기반으로 FPS, millisecond per frame (1프레임당 elapsedTime = deltaTime)을 계산 해 준다.
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((m_pTimer->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		/*std::wostringstream outs;
		outs.precision(6);
		outs << L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_MainHwnd, outs.str().c_str());*/

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void GameProcess::CreateObjects()
{
	/// 지금은 여기서 그리지만 Scene 매니저를 만들어서 걔가 만들도록 하자. 
	// 그래픽스 엔진 (지금은 렌더러)에게 DX오브젝트를 만들어달라고 요청함

	// 그래픽스 엔진에 축을 추가한다.
	m_pD3DEngine->CreateDXObject_Axis();

	// 그래픽스 엔진에 그리드를 추가한다.
	m_pD3DEngine->CreateDXObject_Grid();

}

bool GameProcess::Frame()
{
	return true;
}

LRESULT CALLBACK GameProcess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
	{
		// Save the new client area dimensions.
		int _screenWidth = LOWORD(lParam);
		int _screenHeight = HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
		{
			// 해결!
			// width height 0이 들어가면 터진다.
		}
		else
		{
			if (m_pD3EGinstance)
			{
				m_pD3EGinstance->OnResize(_screenWidth, _screenHeight);
			}
		}
	}
	return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_pD3EGinstance->OnMouseDown(/*wParam, */LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		m_pD3EGinstance->OnMouseUp(/*wParam, */LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_MOUSEMOVE:
		m_pD3EGinstance->OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));


	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

