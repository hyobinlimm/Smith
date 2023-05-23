#pragma once

#include "Scene.h"
#include "HimeEngine.h"

//class Scene;
//class HimeEngine;



class SceneManager
{
public:
	SceneManager() = default;

	~SceneManager() = default;

	SceneManager(const SceneManager& other) = delete;

	SceneManager& operator= (const SceneManager& other) = delete;

	SceneManager(SceneManager&& other) = delete;

public:
	static shared_ptr<SceneManager> GetInstance();

	void Initialize();

	template <typename T>
	void AddGameScene(wstring sceneName);

	void LoadScene(wstring sceneName);

	void Awake();
	void Start();
	void Update();
	void Render();

	void SetGraphicsEngine(HimeEngine* himeEngine);
	void SetScreen(float width, float height) { m_ScreenWidth = width; m_ScreenHeight = height; }
	vector<shared_ptr<Scene>> m_SceneList;
private:
	static shared_ptr<SceneManager> g_SceneInstance;


public:
	HimeEngine* m_HimeEngine;
	shared_ptr<Scene> m_CurScene;
	
	float m_ScreenWidth;
	float m_ScreenHeight;
};

template<typename T>
void SceneManager::AddGameScene(wstring sceneName)
{
	shared_ptr<Scene> _scene = make_shared<T>(sceneName);
	m_SceneList.emplace_back(_scene);
}