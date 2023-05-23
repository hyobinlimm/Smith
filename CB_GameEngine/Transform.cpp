#include "CBpch.h"
#include "Transform.h"

Transform::Transform()
	: m_Parent(nullptr)
{
	m_LMPosition = XMMatrixIdentity();
	m_LMRotateX = XMMatrixIdentity();
	m_LMRotateY = XMMatrixIdentity();
	m_LMRotateZ = XMMatrixIdentity();
	m_LMRotate = XMMatrixIdentity();
	m_LMScale = XMMatrixIdentity();

	m_WMPosition = XMMatrixIdentity();
	m_WMRotateX = XMMatrixIdentity();
	m_WMRotateY = XMMatrixIdentity();
	m_WMRotateZ = XMMatrixIdentity();
	m_WMRotate = XMMatrixIdentity();
	m_WMScale = XMMatrixIdentity();

	m_WorldMatrix = XMMatrixIdentity();
	m_LocalMatrix = XMMatrixIdentity();

	m_LocalPosition = Vector4::Zero;
	m_LocalRotate = Vector4::Zero;
	m_LocalScale = Vector4::One;

	m_WorldPosition = Vector4::Zero;
	m_WorldRotate = Vector4::Zero;
	m_WorldScale = Vector4::One;

	m_Up = Vector4(Vector3::Up);
	m_Right = Vector4(Vector3::Right);
	m_Forward = Vector4(Vector3::Forward);

	m_Name = L"Transform";
}

void Transform::Awake()
{
}

void Transform::Start()
{

}

void Transform::Update()
{
	XMMATRIX scaleMatrix = XMMatrixScaling(m_LocalScale.x, m_LocalScale.y, m_LocalScale.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_LocalRotate.x, m_LocalRotate.y, m_LocalRotate.z);
	XMMATRIX positionMatrix = XMMatrixTranslation(m_LocalPosition.x, m_LocalPosition.y, m_LocalPosition.z);

	m_LMScale = scaleMatrix;
	m_LMRotate = rotationMatrix;
	m_LMPosition = positionMatrix;

	{
		XMVECTOR tempRotate, tempForward, tempUp, tempRight;

		tempRotate = XMVectorSet(m_LocalRotate.x, m_LocalRotate.y, m_LocalRotate.z, 0.0f);

		XMMATRIX temp =
			XMMatrixTranslationFromVector(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)) *
			rotationMatrix;

		tempForward = XMVectorSet(temp.r[3].vector4_f32[0], temp.r[3].vector4_f32[1], temp.r[3].vector4_f32[2], 0.0f);

		temp =
			XMMatrixTranslationFromVector(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) *
			rotationMatrix;

		tempUp = XMVectorSet(temp.r[3].vector4_f32[0], temp.r[3].vector4_f32[1], temp.r[3].vector4_f32[2], 0.0f);

		tempUp = XMVector3Normalize(tempUp);
		tempForward = XMVector3Normalize(tempForward);
		tempRight = XMVector3Cross(tempUp, tempForward);

		m_Up.x = tempUp.vector4_f32[0];
		m_Up.y = tempUp.vector4_f32[1];
		m_Up.z = tempUp.vector4_f32[2];
		m_Up.w = tempUp.vector4_f32[3];

		m_Forward.x = tempForward.vector4_f32[0];
		m_Forward.y = tempForward.vector4_f32[1];
		m_Forward.z = tempForward.vector4_f32[2];
		m_Forward.w = tempForward.vector4_f32[3];

		m_Right.x = tempRight.vector4_f32[0];
		m_Right.y = tempRight.vector4_f32[1];
		m_Right.z = tempRight.vector4_f32[2];
		m_Right.w = tempRight.vector4_f32[3];
	}

	m_LocalMatrix = m_LMScale * m_LMRotate * m_LMPosition;

	if (m_Parent == nullptr)
	{
		m_WorldMatrix = m_LocalMatrix;
	}
	else
	{
		m_WorldMatrix = m_LocalMatrix * m_Parent->m_WorldMatrix;
	}
}



DirectX::SimpleMath::Vector3 Transform::GetWorldPosition()
{
	return Vector3(m_WorldPosition);
}

DirectX::SimpleMath::Vector3 Transform::GetWorldRotate()
{
	return Vector3(m_WorldRotate);
}

DirectX::SimpleMath::Vector3 Transform::GetWorldScale()
{
	return Vector3(m_WorldScale);
}

DirectX::SimpleMath::Vector3 Transform::GetLocalPosition()
{
	return Vector3(m_LocalPosition);
}

DirectX::SimpleMath::Vector3 Transform::GetLocalRotate()
{
	return Vector3(m_LocalRotate);
}

DirectX::SimpleMath::Vector3 Transform::GetLocalScale()
{
	return Vector3(m_LocalScale);
}

void Transform::SetWorldTranslate(const Vector3& pos)
{
	//Vector4 p = pos;

	Vector4 p = { pos.x, pos.y, pos.z, 1 };

	m_WorldPosition += p;

	if (m_Parent == nullptr)
	{
		m_LocalPosition += p;

		m_LMPosition._41 = m_LocalPosition.x;
		m_LMPosition._42 = m_LocalPosition.y;
		m_LMPosition._43 = m_LocalPosition.z;
	}
	else
	{

	}

	UpdateTransform();
}

void Transform::SetWorldRotate(const Vector3& rotate)
{
	//Vector4 r = rotate;

	Vector4 r = {rotate.x, rotate.y, rotate.z, 1};

	m_WorldRotate = r;

	if (m_Parent == nullptr)
	{
		m_LocalRotate = m_WorldRotate;
	}
	else
	{

	}
}

void Transform::SetWorldScale(const Vector3& scale)
{
	//Vector4 s = scale;

	Vector4 s = {scale.x, scale.y, scale.z, 1};

	m_WorldScale = s;

	if (m_Parent == nullptr)
	{
		m_LocalScale = m_WorldScale;
	}
	else
	{

	}
}

void Transform::SetLocalTranslate(const Vector3& pos)
{
	//Vector4 p = pos;

	Vector4 p = { pos.x, pos.y, pos.z, 1 };

	m_LocalPosition = p;

	if (m_Parent == nullptr)
	{
		m_WorldPosition = m_LocalPosition;

		m_LMPosition._41 = m_LocalPosition.x;
		m_LMPosition._42 = m_LocalPosition.y;
		m_LMPosition._43 = m_LocalPosition.z;
	}
	else
	{

	}

	UpdateTransform();
}

void Transform::SetLocalPosition(const Vector3& pos)
{
	//Vector4 p = pos;

	Vector4 p = { pos.x, pos.y, pos.z, 1 };
	m_LocalPosition = p;

	XMMATRIX positionMatrix = XMMatrixTranslation(m_LocalPosition.x, m_LocalPosition.y, m_LocalPosition.z);
	XMStoreFloat4x4(&m_LMPosition, positionMatrix);

	if (m_Parent == nullptr)
	{
		m_WorldPosition = m_LocalPosition;
	}
	else
	{
		m_WMPosition = m_LMPosition * m_Parent->m_WorldMatrix;
	}

	UpdateTransform();
}

void Transform::SetLocalRotate(const Vector3& rotate)
{
	//Vector4 r = rotate;

	Vector4 r = { rotate.x, rotate.y, rotate.z, 1 };

	m_LocalRotate = r;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_LocalRotate.x, m_LocalRotate.y, m_LocalRotate.z);
	XMStoreFloat4x4(&m_LMRotate, rotationMatrix);
	if (m_Parent == nullptr)
	{
		m_WorldRotate = m_LocalRotate;
		m_WMRotate = m_LMRotate;
	}
	else
	{
		m_WorldRotate = m_LocalRotate - m_Parent->m_WorldRotate;
	}

	UpdateTransform();
}

void Transform::SetLocalScale(const Vector3& scale)
{
	//Vector4 s = scale;

	Vector4 s = { scale.x, scale.y, scale.z, 1 };
	m_LocalScale = s;
	m_LMScale._11 = m_LocalScale.x;
	m_LMScale._22 = m_LocalScale.y;
	m_LMScale._33 = m_LocalScale.z;

	if (m_Parent == nullptr)
	{
		m_WMScale = m_LMScale;
	}
	else
	{
		m_WMScale = m_LMScale * m_Parent->m_WorldMatrix;
	}

	UpdateTransform();
}

void Transform::Translate(const Vector3& pos)
{
	//Vector4 p = pos;

	Vector4 p = { pos.x, pos.y, pos.z, 1 };
	m_WorldPosition += p;

	if (m_Parent == nullptr)
	{
		m_LocalPosition = m_WorldPosition;
	}
	else
	{

	}
}

void Transform::Rotate(const Vector3& rotate)
{
	//Vector4 r = rotate;

	Vector4 r = { rotate.x, rotate.y, rotate.z, 1 };
	m_LocalRotate += r;

	if (m_Parent == nullptr)
	{
		m_WorldRotate = m_LocalRotate;
	}
	else
	{

	}
}

void Transform::UpdateTransform()
{
	m_LocalMatrix = m_LMScale * m_LMRotate * m_LMPosition;

	if (m_Parent == nullptr)
	{
		m_WorldMatrix = m_LocalMatrix;
	}
	else
	{
		m_WorldMatrix = m_LocalMatrix * m_Parent->m_WorldMatrix;
	}
}

void Transform::LookAt(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	m_Forward = XMVector3Normalize(XMVectorSubtract(T, P));
	m_Right = XMVector3Normalize(XMVector3Cross(U, m_Forward));
	m_Up = XMVector3Cross(m_Forward, m_Right);

	m_WorldPosition = P;

	m_WMRotateX = XMMatrixRotationAxis(m_Right, 0);
	m_WMRotateY = XMMatrixRotationAxis(m_Up, 0);
	m_WMRotateZ = XMMatrixRotationAxis(m_Forward, 0);

	m_WorldRotate.x = atan2f(m_Up.Dot(Vector4(0, 0, 1, 0)), m_Up.Dot(Vector4(0, 1, 0, 0))) * Rad2Deg;
	m_WorldRotate.y = atan2f(m_Forward.Dot(Vector4(1, 0, 0, 0)), m_Forward.Dot(Vector4(0, 0, 1, 0))) * Rad2Deg;
	m_WorldRotate.z = atan2f(m_Right.Dot(Vector4(0, 1, 0, 0)), m_Right.Dot(Vector4(1, 0, 0, 0))) * Rad2Deg;

	if (m_Parent == nullptr)
	{
		m_LMRotateX = m_WMRotateX;
		m_LMRotateY = m_WMRotateY;
		m_LMRotateZ = m_WMRotateZ;

		m_WMRotate = m_WMRotateX * m_WMRotateY * m_WMRotateZ;
		m_LMRotate = m_WMRotate;
	}
	else
	{

	}
}

void Transform::SetParent(shared_ptr<Transform> parent)
{
	if (parent == nullptr || m_Parent == parent)
		return;
	m_Parent = parent;
	m_Parent->AddChild(this);
}

void Transform::SetParent(shared_ptr<GameObject> parent)
{
	if (parent == nullptr)
		return;

	shared_ptr<Transform> parentTransform = parent->GetComponent<Transform>();

	if (parentTransform == nullptr || m_Parent == parentTransform)
		return;

	m_Parent = parentTransform;
	m_Parent->AddChild(this);
}

void Transform::AddChild(Transform* child)
{
	m_Childs.push_back(child);
}
