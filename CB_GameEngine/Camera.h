#pragma once

#include "Component.h"
#include "HimeRendererStructs.h"

/// <summary>
/// 2022.08.03
/// ī�޶� ���� ������Ʈ�� ���� ������Ʈ�� �ٿ�����.
/// �׷��Ƚ��������� �޾ƾ��ϴ� �������� ���� ����ü�� ����� �� ����ü�� CameraInfo����
/// �̸��� ������ �Ἥ ����� ���� ������?
/// </summary>
class GameObject;

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera() {};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	//void SetPlayer(shared_ptr<GameObject> playerObj) { m_PlayerObj = playerObj; }

	void SetCameraInfo(shared_ptr<CameraInfo> pCameraInfo) {
		m_CameraInfo = pCameraInfo;
	}

	void SetCameraTargetPos(Vector3 targetpos);
	void SetCameraZoom(int zoom);


	float GetZoom();

	shared_ptr<CameraInfo> GetCameraInfo() { return m_CameraInfo; }

private:
	// ī�޶�� ���ο� Transform�� �־ ������ �غ���.

	shared_ptr<CameraInfo> m_CameraInfo;
	Vector3 m_TargetPos;
	float m_Zoom;
};

