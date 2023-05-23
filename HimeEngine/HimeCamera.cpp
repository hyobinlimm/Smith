#include "HimeCamera.h"
#include "MathHelper.h"

HimeCamera::HimeCamera()
	: m_Position(0.0f, 0.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Look(0.0f, 0.0f, 1.0f)
{
	SetLens(0.25f * MathHelper::Pi, 1.0f, 0.000001f, 1000.0f);
}

HimeCamera::~HimeCamera()
{

}

void HimeCamera::Update(float deltaTime)
{
	if (GetAsyncKeyState('W') & 0x8000)
		Walk(10.0f * deltaTime);

	if (GetAsyncKeyState('S') & 0x8000)
		Walk(-10.0f * deltaTime);

	if (GetAsyncKeyState('A') & 0x8000)
		Strafe(-10.0f * deltaTime);

	if (GetAsyncKeyState('D') & 0x8000)
		Strafe(10.0f * deltaTime);

	if (GetAsyncKeyState('Q') & 0x8000)
		WorldUpDown(-10.0f * deltaTime);

	if (GetAsyncKeyState('E') & 0x8000)
		WorldUpDown(10.0f * deltaTime);

	UpdateViewMatrix();
}

XMVECTOR HimeCamera::GetPositionXM() const
{
	return XMLoadFloat3(&m_Position);
}

XMFLOAT3 HimeCamera::GetPosition() const
{
	return m_Position;
}

void HimeCamera::SetPosition(float x, float y, float z)
{
	m_Position = XMFLOAT3(x, y, z);
}

void HimeCamera::SetPosition(const XMFLOAT3& v)
{
	m_Position = v;
}

XMVECTOR HimeCamera::GetRightXM() const
{
	return XMLoadFloat3(&m_Right);
}

XMFLOAT3 HimeCamera::GetRight() const
{
	return m_Right;
}

XMVECTOR HimeCamera::GetUpXM() const
{
	return XMLoadFloat3(&m_Up);
}

XMFLOAT3 HimeCamera::GetUp() const
{
	return m_Up;
}

XMVECTOR HimeCamera::GetLookXM() const
{
	return XMLoadFloat3(&m_Look);
}

XMFLOAT3 HimeCamera::GetLook() const
{
	return m_Look;
}

float HimeCamera::GetNearZ() const
{
	return m_NearZ;
}

float HimeCamera::GetFarZ() const
{
	return m_FarZ;
}

float HimeCamera::GetAspect() const
{
	return m_Aspect;
}

float HimeCamera::GetFovY() const
{
	return m_FovY;
}

float HimeCamera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_NearZ);
}

float HimeCamera::GetNearWindowWidth() const
{
	return m_Aspect * m_NearWindowHeight;
}

float HimeCamera::GetNearWindowHeight() const
{
	return m_NearWindowHeight;
}

float HimeCamera::GetFarWindowWidth() const
{
	return m_Aspect * m_FarWindowHeight;
}

float HimeCamera::GetFarWindowHeight() const
{
	return m_FarWindowHeight;
}

void HimeCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Proj, P);
}

void HimeCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	// 시선벡터와 월드업을 외적하면 right벡터가 나오고
	// 다시 그 right벡터와 시선벡터를 외적하면 내 up벡터가 나옴
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}

void HimeCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);
}

XMMATRIX HimeCamera::View() const
{
	return XMLoadFloat4x4(&m_View);
}

XMMATRIX HimeCamera::Proj() const
{
	return XMLoadFloat4x4(&m_Proj);
}

XMMATRIX HimeCamera::ViewProj() const
{
	return XMMatrixMultiply(View(), Proj());
}

void HimeCamera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));
}

void HimeCamera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}

void HimeCamera::WorldUpDown(float d)
{
	// mPosition += d * mUp
	XMVECTOR scale = XMVectorReplicate(d);
	XMVECTOR up = XMLoadFloat3(&m_Up);
	XMVECTOR position = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(scale, up, position));
}

void HimeCamera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void HimeCamera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void HimeCamera::UpdateViewMatrix(CameraInfo* pCameraInfo)
{
	//XMVECTOR R = XMLoadFloat3(&m_Right);
	//XMVECTOR U = XMLoadFloat3(&m_Up);
	//XMVECTOR L = XMLoadFloat3(&m_Look);
	//XMVECTOR P = XMLoadFloat3(&m_Position);

	XMVECTOR R = XMLoadFloat3(&pCameraInfo->Right);
	XMVECTOR U = XMLoadFloat3(&pCameraInfo->Up);
	XMVECTOR L = XMLoadFloat3(&pCameraInfo->Look);
	XMVECTOR P = XMLoadFloat3(&pCameraInfo->Pos);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;
}

void HimeCamera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;
}
