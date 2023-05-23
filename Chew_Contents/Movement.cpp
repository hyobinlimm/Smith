#include "CCPch.h"
#include "GameTimer.h"
#include "Transform.h"
#include "ControllerEnum.h"
#include "Movement.h"
#include "SoundManager.h"
#include "MeshRenderer.h"

Movement::Movement()
{
	m_Name = L"Movement";
}

Movement::~Movement()
{

}

void Movement::RotateObject(int dir)
{
	Vector3 _VRotation = m_Transform->GetLocalRotate();
	Vector3 _VDirection;

	// �ּ�ó�� �� �κ��� ������ ġ�찡 �밢������ �����̵��� �ۼ��� �κ�
	switch (dir)
	{
	case 1:
		_VDirection = { 0,PI, 0 };
		break;
	case DOWN:
		_VDirection = { 0, PI / 2 + PI / 4, 0 };
		break;
	case 3:
		_VDirection = { 0, PI / 2, 0 };
		break;
	case LEFT:
		_VDirection = { 0,-PI / 4 - PI / 2,0 };
		break;
	case 5:
		_VDirection = _VRotation;
		break;
	case RIGHT:
		_VDirection = { 0, PI / 4, 0 };
		break;
	case 7:
		_VDirection = { 0, -(PI / 2), 0 };
		break;
	case UP:
		_VDirection = { 0,-PI / 4,0 };
		break;
	case 9:
		_VDirection = { 0, 0, 0 };
		break;
	default:
		break;
	}

	//������Ʈ�� ���� �����̼��� ���ʹϾ����� ��ȯ
	Quaternion _QRotation = Quaternion::CreateFromYawPitchRoll(_VRotation.y, _VRotation.x, _VRotation.z);
	//Quaternion _QRotation = Quaternion::CreateFromYawPitchRoll(_VRotation.z, _VRotation.y, _VRotation.x);
	//�����ϴ� ������ ���ʹϾ����� ��ȯ
	Quaternion _Qdirection = Quaternion::CreateFromYawPitchRoll(_VDirection.y, _VDirection.x, _VDirection.z);
	//Quaternion _Qdirection = Quaternion::CreateFromYawPitchRoll(_VDirection.z, _VDirection.y, _VDirection.x);

	//�ε巯�� ȸ���� ���� ���鼱�������� ������ ���ο� ȸ���� ����
	Quaternion _newRotation = Quaternion::Slerp(_QRotation, _Qdirection, 0.1);

	//����� ȸ���� ����
	m_Transform->SetLocalRotate(Quat2Vec(_newRotation));
}

void Movement::MoveObject()
{
	//�ٶ󺸴� ���͸� ����
	Vector3 v = { m_Transform->m_Forward.x, m_Transform->m_Forward.y, m_Transform->m_Forward.z };

	SoundManager::GetInstance()->CharPlay("Char_Walk");
	//SoundManager::GetInstance()->SFXPlay("Char_Walk");
	 
	//�ٶ󺸴� �������� ����
	m_Transform->Translate(v*m_MoveSpeed);
}

void Movement::SetMoveSpeed(float amount)
{
	m_MoveSpeed = amount;
}

void Movement::SetRotateSpeed(float amount)
{
	m_RotateSpeed = amount;
}

DirectX::SimpleMath::Vector3 Movement::Quat2Vec(Quaternion& quat)
{
	//������ Comet ����!
	// ���� : Z / X / Y �� ������ ȸ���ϴ� �Ϳ� ���ؼ� �����.
	float x = quat.x;
	float y = quat.y;
	float z = quat.z;
	float w = quat.w;

	float sqx = x * x;
	float sqy = y * y;
	float sqz = z * z;
	float sqw = w * w;

	// C++���� asin�� �Ű������� -1 ~ 1 ���̷θ� �޽��ϴ�. (�� �̻��� �� ���� �и� 90�� �Ǵ� -90���� Ư����.)
	float exceptCheck = 2.0f * (w * x - y * z);

	float eulerX = 0.f;

	if (abs(exceptCheck) >= 1.f)
		// copysign : _excepCheck�� ��ȣ�� 2���� ����, 90���� ��ȯ�Ѵ�.
		eulerX = copysign(3.1415926535f / 2, exceptCheck);
	else
		eulerX = exceptCheck;

	float eulerY = atan2(2.0f * (x * z + w * y), (-sqx - sqy + sqz + sqw));

	float eulerZ = atan2(2.0f * (x * y + z * w), (-sqx + sqy - sqz + sqw));

	return Vector3(eulerX, eulerY, eulerZ);
}

