#include "CBpch.h"
#include "SceneManager.h"

shared_ptr<SceneManager> SceneManager::g_SceneInstance = nullptr;

std::shared_ptr<SceneManager> SceneManager::GetInstance()
{
	if (g_SceneInstance == nullptr)
		g_SceneInstance = make_shared<SceneManager>();

	return g_SceneInstance;
}

void SceneManager::Initialize()
{
	m_CurScene = nullptr;
}

void SceneManager::LoadScene(wstring sceneName)
{
	for (size_t i = 0; i < m_SceneList.size(); i++)
	{
		if (m_SceneList[i]->m_Name == sceneName)
		{
			m_CurScene = m_SceneList[i];
			m_CurScene->InitScene();
	
		}
	}
}

void SceneManager::Awake()
{
	m_CurScene->Awake();
}

void SceneManager::Start()
{
	m_CurScene->Start();
}

void SceneManager::Update()
{
	m_CurScene->Update();
}

void SceneManager::Render()
{
	m_CurScene->Render();
}

void SceneManager::SetGraphicsEngine(HimeEngine* himeEngine)
{
	m_HimeEngine = himeEngine;
}
