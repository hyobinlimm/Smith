#include "CBpch.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Camera.h"
#include "CameraController.h"

CameraController::CameraController()
	:m_ZoomSpeed(0.5f), m_MoveSpeed(0.1f), m_CameraMode(0), m_LookPostion(0, 0, 0)
{
	TRACE("Create\n");
}

void CameraController::Update()
{

	if (m_CameraMode == 0)
	{
		if (IsPressed(VirtualKey::Key_W) || IsPressed(VirtualKey::Key_S) || IsPressed(VirtualKey::Key_A) || IsPressed(VirtualKey::Key_D))
		{
			TRACE("MOVE\n");
			MoveCamera(MoveDir());
		}
	}
	else
	{

		m_LookPostion = m_TargetTransform->GetWorldPosition();
	}
	if (IsPressed(VirtualKey::Key_Q))
	{
		ZoomCamera(m_ZoomSpeed);
	}

	if (IsPressed(VirtualKey::Key_E))
	{
		ZoomCamera(-m_ZoomSpeed);
	}

	m_Camera->SetCameraTargetPos(m_LookPostion);
}

void CameraController::Start()
{
	for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
	{
		if (obj->GetName() == L"Smith")
		{
			SetCameraTarget(obj);
			SetControllMode(1);
			return;
		}
	}
}

void CameraController::Awake()
{
	m_Input = InputManager::GetInstance();
}

void CameraController::SetCamera(shared_ptr<GameObject> camera)
{
	if (camera->GetComponent<Camera>() != nullptr)
	{
		m_Camera = camera->GetComponent<Camera>();
	}
}

void CameraController::SetControllMode(int mode)
{
	m_CameraMode = mode;
	switch (mode)
	{
	case 0:
		m_Camera->SetCameraTargetPos({ m_LookPostion });
		break;
	case 1:
		m_LookPostion = m_TargetTransform->GetWorldPosition();
		m_Camera->SetCameraTargetPos(m_LookPostion);
		break;
	default:
		break;
	}
}

void CameraController::SetCameraTarget(shared_ptr<GameObject> target)
{
	if (target->GetComponent<Transform>() != nullptr)
	{
		m_TargetTransform = target->GetComponent<Transform>();
	}
}

int CameraController::MoveDir()
{
	if (IsPressed(VirtualKey::Key_W)) { return Up; }
	if (IsPressed(VirtualKey::Key_S)) { return Down; }
	if (IsPressed(VirtualKey::Key_A)) { return Left; }
	if (IsPressed(VirtualKey::Key_D)) { return Right; }
}



void CameraController::MoveCamera(int dir)
{
	switch (dir)
	{
	case Up:
		m_LookPostion += Vector3(-cos(PI / 4), 0, sin(PI / 4)) * m_MoveSpeed;
		break;
	case Down:
		m_LookPostion += Vector3(sin(PI / 4), 0, -cos(PI / 4)) * m_MoveSpeed;
		break;
	case Left:
		m_LookPostion += Vector3(sin(-PI / 4), 0, -cos(-PI / 4)) * m_MoveSpeed;
		break;
	case Right:
		m_LookPostion += Vector3(-sin(-PI / 4), 0, cos(-PI / 4)) * m_MoveSpeed;
		break;
	default:
		break;
	}
}


void CameraController::ZoomCamera(float amount)
{
	static float _camZoom;
	_camZoom += amount * m_ZoomSpeed;

	if (_camZoom >= 10)
	{
		_camZoom = 10;
	}
	else if (_camZoom < 0)
	{
		_camZoom = 0;
	}
	m_Camera->SetCameraZoom(_camZoom);

}


bool CameraController::IsPressed(VirtualKey key)
{
	if (!m_Input) return false;
	bool Ishold = KeyState::Hold == m_Input->GetCurrentKeyState(key) ? true : false;
	bool IsDown = KeyState::Down == m_Input->GetCurrentKeyState(key) ? true : false;
	if (Ishold || IsDown)
	{
		return true;
	}
	return false;
}
