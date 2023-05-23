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

	// 주석처리 된 부분은 이전에 치우가 대각선으로 움직이도록 작성한 부분
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

	//오브젝트의 로컬 로테이션을 쿼터니언으로 변환
	Quaternion _QRotation = Quaternion::CreateFromYawPitchRoll(_VRotation.y, _VRotation.x, _VRotation.z);
	//Quaternion _QRotation = Quaternion::CreateFromYawPitchRoll(_VRotation.z, _VRotation.y, _VRotation.x);
	//가야하는 방향을 쿼터니언으로 변환
	Quaternion _Qdirection = Quaternion::CreateFromYawPitchRoll(_VDirection.y, _VDirection.x, _VDirection.z);
	//Quaternion _Qdirection = Quaternion::CreateFromYawPitchRoll(_VDirection.z, _VDirection.y, _VDirection.x);

	//부드러운 회전을 위해 구면선형보간을 적용한 새로운 회전을 생성
	Quaternion _newRotation = Quaternion::Slerp(_QRotation, _Qdirection, 0.1);

	//계산한 회전을 적용
	m_Transform->SetLocalRotate(Quat2Vec(_newRotation));
}

void Movement::MoveObject()
{
	//바라보는 벡터를 생성
	Vector3 v = { m_Transform->m_Forward.x, m_Transform->m_Forward.y, m_Transform->m_Forward.z };

	SoundManager::GetInstance()->CharPlay("Char_Walk");
	//SoundManager::GetInstance()->SFXPlay("Char_Walk");
	 
	//바라보는 방향으로 전진
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
	//고마워요 Comet 엔진!
	// 주의 : Z / X / Y 축 순으로 회전하는 것에 대해서 만든다.
	float x = quat.x;
	float y = quat.y;
	float z = quat.z;
	float w = quat.w;

	float sqx = x * x;
	float sqy = y * y;
	float sqz = z * z;
	float sqw = w * w;

	// C++에서 asin은 매개변수로 -1 ~ 1 사이로만 받습니다. (그 이상이 된 경우는 분명 90도 또는 -90도인 특이점.)
	float exceptCheck = 2.0f * (w * x - y * z);

	float eulerX = 0.f;

	if (abs(exceptCheck) >= 1.f)
		// copysign : _excepCheck의 부호로 2분의 파이, 90도를 반환한다.
		eulerX = copysign(3.1415926535f / 2, exceptCheck);
	else
		eulerX = exceptCheck;

	float eulerY = atan2(2.0f * (x * z + w * y), (-sqx - sqy + sqz + sqw));

	float eulerZ = atan2(2.0f * (x * y + z * w), (-sqx + sqy - sqz + sqw));

	return Vector3(eulerX, eulerY, eulerZ);
}

