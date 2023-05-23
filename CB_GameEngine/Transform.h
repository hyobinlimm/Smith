#pragma once

#include "Component.h"

/// <summary>
/// 2022.08.03
/// 오브젝트의 이동, 회전, 크기변환 등을 담당할 컴포넌트.
/// </summary>

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform() {};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

public:
	Vector3 GetWorldPosition();
	Vector3 GetWorldRotate();
	Vector3 GetWorldScale();
	Vector3 GetLocalPosition();
	Vector3 GetLocalRotate();
	Vector3 GetLocalScale();

	void SetWorldTranslate(const Vector3& pos);
	void SetWorldRotate(const Vector3& rotate);
	void SetWorldScale(const Vector3& scale);
	void SetLocalTranslate(const Vector3& pos);
	void SetLocalPosition(const Vector3& pos);
	void SetLocalRotate(const Vector3& rotate);
	void SetLocalScale(const Vector3& scale);

	void Translate(const Vector3& pos);
	void Rotate(const Vector3& rotate);
	//void RotateY(const float degree);
	//void Rotate(const float degree);

	void UpdateTransform();
	void LookAt(const Vector3& pos, const Vector3& target, const Vector3& up);

	void SetParent(shared_ptr<Transform> parent);
	void SetParent(shared_ptr<GameObject> parent);
	void AddChild(Transform* child);



public:
	shared_ptr<Transform> m_Parent;
	vector<Transform*> m_Childs;

	Matrix m_LMPosition;
	Matrix m_LMRotateX;
	Matrix m_LMRotateY;
	Matrix m_LMRotateZ;
	Matrix m_LMRotate;
	Matrix m_LMScale;

	Matrix m_WMPosition;
	Matrix m_WMRotateX;
	Matrix m_WMRotateY;
	Matrix m_WMRotateZ;
	Matrix m_WMRotate;
	Matrix m_WMScale;

	Matrix m_WorldMatrix;	// World Matrix
	Matrix m_LocalMatrix;	// Local Matrix

	Vector4 m_LocalPosition;		// Local Position
	Vector4 m_LocalRotate;			// Local Rotate
	Vector4 m_LocalScale;			// Local Scale

	Vector4 m_WorldPosition;		// World Position
	Vector4 m_WorldRotate;			// World Rotate
	Vector4 m_WorldScale;			// World Scale

	Vector4 m_Up;					// Object World Up Vector
	Vector4 m_Right;				// Object World Right Vector
	Vector4 m_Forward;				// Object World Forward Vector

	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);
	const float Deg2Rad = (PI * 2) / 360;
};

