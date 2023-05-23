#pragma once

#include "Component.h"

/// <summary>
/// AABB구현작업
/// </summary>

class GameObject;
struct MeshBuildInfo;

class BoxCollider : public Component
{
public:
	BoxCollider();
	virtual ~BoxCollider() {};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	shared_ptr<GameObject> GetIAObj() {
		if (!m_InterActionObjList.empty())
			return m_InterActionObjList.back();
	}

private:
	void VolumeUpdate();
	bool CollisionCheck(shared_ptr<GameObject> otherObj);
	void MoveReverse();
	void MoveInside();
	bool ObjectCheck(shared_ptr<GameObject> otherObj);
	void OnFloor(shared_ptr<GameObject> otherObj);

private:
	MeshBuildInfo* m_BuildInfo;
	Vector3 m_CollisionVec;
	Vector3 m_InsideVec;
	bool m_IsCrash;
	bool m_IsOnFloor;
	Vector3 m_PrevPos;
	bool m_IsTriggerOn;
	int m_FloorCount;

public:
	vector<shared_ptr<GameObject>> m_InterActionObjList;
};

