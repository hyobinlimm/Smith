#pragma once
#include "Component.h"

class InputManager;
class GameObject;
class Transform;
class Camera;
class CameraController : public Component
{
public:
	CameraController();
	~CameraController() {};

	virtual void Update() override;
	void Start();
	void Awake();

public:
	void SetCamera(shared_ptr<GameObject> camera);
	void SetControllMode(int mode);
	void SetCameraTarget(shared_ptr<GameObject> target);

	int MoveDir();
	void MoveCamera(int dir);
	void ZoomCamera(float amount);
private:
	float m_ZoomSpeed;
	float m_MoveSpeed;
	shared_ptr<Camera> m_Camera;
	shared_ptr<InputManager> m_Input;
	shared_ptr<Transform> m_TargetTransform;
	Vector3 m_LookPostion;
	bool IsPressed(VirtualKey key);
	int m_CameraMode;

private:
	enum eDirection
	{
		None,
		Up,
		Down,
		Left,
		Right
	};

	//라디안 계산용 변수
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);
	const float Deg2Rad = (PI * 2) / 360;

};

