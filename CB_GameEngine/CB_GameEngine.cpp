#include "CBpch.h"
#include "CB_GameEngine.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "GameTimer.h"
#include "HimeEngine.h"
#include "ObjectDataTable.h"
#include "SoundManager.h"
#include "UIManager.h"

CB_GameEngine::CB_GameEngine()
	:m_HimeEngine(nullptr)
{
}

CB_GameEngine::~CB_GameEngine()
{
	delete m_HimeEngine;
}

void CB_GameEngine::Init(HWND hWnd, int width, int height)
{
	InputManager::GetInstance()->Initialize(hWnd);
	GameTimer::GetInstance()->Reset();

	// 사운드매니저로 사운드 JSON파일 로딩.
	//SoundManager::GetInstance()->Initialize("../Resource/DataTable/SoundTest.json");
	SoundManager::GetInstance()->Initialize("../Resource/DataTable/SoundData.json");

	UIManager::GetInstance()->Initialize();

	m_ObjData = make_shared<ObjectDataTable>();
	//m_ObjData->DeserializeFromFile("../Resource/DataTable/data.json");
	m_ObjData->DeserializeFromFile("../Resource/DataTable/StageTest.json");
	//m_ObjData->DeserializeFromFile("../Resource/DataTable/GameSceneTest.json");

	for (size_t i = 0; i < m_ObjData->m_DataCount; i++)
	{
		FilePath* _NewFile = new FilePath();
		_NewFile->ID_name = m_ObjData->m_Name[i];
		_NewFile->FileName = m_ObjData->m_FileName[i];
		_NewFile->TexName = m_ObjData->m_TexName[i];
		m_FilePath.emplace_back(_NewFile);
	}
	m_HimeEngine = new HimeEngine();
	m_HimeEngine->Initialize(hWnd, width, height, SceneManager::GetInstance()->m_CurScene->GetCameraInfo().get());
	m_HimeEngine->SetBuildInfoAll(m_FilePath);
	m_BuildInfo = m_HimeEngine->GetMeshInfoList();

	for (auto& _scene : SceneManager::GetInstance()->m_SceneList)
	{
		if (_scene->m_Name == L"TestScene")
		{
			_scene->LoadObject(m_BuildInfo, m_ObjData);
		}
		
	}
		/*m_CurScene->LoadObject(m_BuildInfo, m_ObjData);*/

	SceneManager::GetInstance()->SetGraphicsEngine(m_HimeEngine);
	SceneManager::GetInstance()->SetScreen(width, height);
}

void CB_GameEngine::Update()
{
	SceneManager::GetInstance()->Update();
	GameTimer::GetInstance()->Tick();
	GameTimer::GetInstance()->DeltaTime();  
	InputManager::GetInstance()->Update();

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((GameTimer::GetInstance()->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		/*
		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());
		*/

		for (auto& text : SceneManager::GetInstance()->m_CurScene->m_stringList)
		{
			if (text->IDName == L"FPS")
			{
				text->value = fps;
			}

			else if (text->IDName == L"Frame Time")
			{
				text->value = mspf;
			}
		}

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void CB_GameEngine::Render()
{
	SceneManager::GetInstance()->m_CurScene->Render();

	m_HimeEngine->Render();
}

void CB_GameEngine::SetScreen(WPARAM wparam, LPARAM lparam, bool resizing, bool sizeMove)
{
	if (m_HimeEngine != nullptr)
	{
		if (sizeMove)
		{
			m_HimeEngine->OnResize();
			return;
		}

		m_HimeEngine->SetScreenSize(LOWORD(lparam), HIWORD(lparam));
		SceneManager::GetInstance()->SetScreen(LOWORD(lparam), HIWORD(lparam));
		if (SceneManager::GetInstance()->m_CurScene != nullptr)
		{
			for (auto& text : SceneManager::GetInstance()->m_CurScene->m_stringList)
			{
				if (text->IDName == L"Screen")
				{
					text->value = LOWORD(lparam);
					text->value2 = HIWORD(lparam);
				}
			}
		}

		if (wparam == SIZE_MINIMIZED)
		{

		}

		else if (wparam == SIZE_MAXIMIZED)
		{
			m_HimeEngine->OnResize();
		}

		else if (wparam == SIZE_RESTORED)
		{
			if (resizing)
			{

			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				m_HimeEngine->OnResize();
			}
		}
	}
}
