#include "CBpch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Object.h"
#include "HimeEngine.h"
#include "Renderer.h"
#include "MeshRenderer.h"
#include "..\Chew_Contents\Controller.h"
#include "CameraController.h"
#include "Transform.h"
#include "GameTimer.h"
#include "Camera.h"
#include "TextRenderer.h"
#include "ObjectDataTable.h"
#include "..\Chew_Contents\UIRenderer.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "WeaponManager.h"
#include "WeaponTypeDataTable.h"
#include "MatDataTable.h"
#include "EnchantDataTable.h"
#include "HandleDataTable.h"

Scene::Scene(wstring sceneName)
	: m_Name(sceneName), m_SceneTimer(0), m_SceneDate(1)
{
	InitScene();

}
void Scene::InitScene()
{
	int _x = 50;
	int _y = 100;
	float _size = 2.f;
	XMFLOAT4 _color{ 1.f, 1.f, 1.f, 1.f };

	m_CameraInfo = make_shared<CameraInfo>();

	// 메인 카메라
	m_MainCamera = make_shared<GameObject>(L"MainCamera");
	m_MainCamera->AddComponent<Transform>();
	m_MainCamera->AddComponent<Camera>();
	//m_MainCamera->GetComponent<Camera>()->SetPlayer(nullptr);

	m_MainCamera->GetComponent<Camera>()->SetCameraInfo(m_CameraInfo);

	m_CameraInfo->Pos = { 15, 15, -15 };
	m_CameraInfo->Up = { 0, 1.0, 0 };
	m_CameraInfo->Target = { 0, 0, 0 };

	m_MainCamera->GetComponent<Transform>()->SetLocalPosition(m_CameraInfo->Pos);
	m_MainCamera->GetComponent<Transform>()->LookAt(m_CameraInfo->Pos, m_CameraInfo->Target, m_CameraInfo->Up);
	m_MainCamera->AddComponent<TextRenderer>();
	m_MainCamera->GetComponent<TextRenderer>()->SetTexData(m_MainCamera->GetName(), _x, _y, _size, _color, false, L"Camera pos: %.2f, %.2f, %.2f",
		m_MainCamera->GetComponent<Camera>().get()->GetCameraInfo().get()->Pos.x, m_MainCamera->GetComponent<Camera>().get()->GetCameraInfo().get()->Pos.y, m_MainCamera->GetComponent<Camera>().get()->GetCameraInfo().get()->Pos.z);

	m_GameObjectList.emplace_back(m_MainCamera);

	m_TestController = make_shared<GameObject>(L"TestController");
	m_TestController->AddComponent<CameraController>();
	m_TestController->GetComponent<CameraController>()->SetCamera(m_MainCamera);
	m_GameObjectList.emplace_back(m_TestController);
}