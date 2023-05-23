#pragma once

#include "Component.h"
#include "HimeRendererStructs.h"

/// <summary>
/// 2022.08.03
/// 카메라 또한 컴포넌트로 만들어서 오브젝트에 붙여보자.
/// 그래픽스엔진에서 받아야하는 정보들을 담은 구조체를 만들고 그 구조체를 CameraInfo같은
/// 이름의 형식을 써서 만들면 되지 않을까?
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
	// 카메라는 내부에 Transform을 넣어서 관리를 해보자.

	shared_ptr<CameraInfo> m_CameraInfo;
	Vector3 m_TargetPos;
	float m_Zoom;
};

