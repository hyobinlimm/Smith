#include "CBpch.h"
#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"
#include "InputManager.h"

Camera::Camera()
	: m_Zoom(0), m_TargetPos({0,0,0})
{

}

void Camera::Awake()
{
}

void Camera::Start()
{

}

void Camera::Update()
{
	Vector3 _Dis = {15 - m_Zoom, 15 - m_Zoom, -15 + m_Zoom };
	m_Transform->SetLocalPosition(m_TargetPos + _Dis);

	m_CameraInfo->Pos = m_Transform->GetWorldPosition();
	m_CameraInfo->Up = { 0, 1.0, 0 };
	m_CameraInfo->Target = m_TargetPos;

	
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&m_CameraInfo->Target), XMLoadFloat3(&m_CameraInfo->Pos)));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_CameraInfo->Up), L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_CameraInfo->Look, L);
	XMStoreFloat3(&m_CameraInfo->Right, R);
	XMStoreFloat3(&m_CameraInfo->Up, U);
}

void Camera::SetCameraTargetPos(Vector3 targetpos)
{
	m_TargetPos = targetpos;
}

void Camera::SetCameraZoom(int zoom)
{
	m_Zoom = zoom;
}

float Camera::GetZoom()
{
	return m_Zoom;
}

