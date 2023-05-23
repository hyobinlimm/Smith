#pragma once

#include "HimeRendererStructs.h"

class GameObject;
class HimeEngine;
class ObjectDataTable;
enum class GameMode;

class Scene
{
public:
	Scene(wstring sceneName);

	void InitScene();

	virtual ~Scene() {};

public:
	virtual void Awake() abstract;
	virtual void Start() abstract;
	virtual void Update() abstract;
	virtual void Render() abstract;

	virtual void LoadObject(vector<MeshBuildInfo*> MeshInfoList, shared_ptr<ObjectDataTable> ObjData) abstract;

	shared_ptr<CameraInfo> GetCameraInfo() { return m_CameraInfo; }

protected:
	virtual void CreateUI() abstract;

protected:
	shared_ptr<CameraInfo> m_CameraInfo;

public:
	HimeEngine* m_HimeEngine;
	wstring m_Name;
	vector<shared_ptr<GameObject>> m_GameObjectList;
	vector<shared_ptr<GameObject>> m_RenderGameObjectList;
	shared_ptr<GameObject> m_MainCamera;
	shared_ptr<GameObject> m_HyobinTest;
	shared_ptr<GameObject> m_TestController;

	vector<UIData*> m_TexList;
	
	vector<shared_ptr<TextData>> m_stringList;


	shared_ptr<TextData> m_SmithData;

	shared_ptr<TextData> sceneTime;

	float m_SceneTimer;
	int m_SceneDate;

};

