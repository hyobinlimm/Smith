#include "CBpch.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "HimeRendererStructs.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "SceneManager.h"
#include "../Chew_Contents/Interactable.h"

BoxCollider::BoxCollider()
	: m_IsCrash(false), m_IsOnFloor(true), m_PrevPos{ 0, 0, 0 }, m_IsTriggerOn(false), m_FloorCount(1)
{

}

void BoxCollider::Awake()
{
	if (m_pGameObject->GetComponent<MeshRenderer>() != nullptr)
	{
		m_BuildInfo = m_pGameObject->GetComponent<MeshRenderer>()->GetMeshInfo();
	}
}

void BoxCollider::Start()
{

}

void BoxCollider::Update()
{
	//m_CollisionVec = {0,0,0};
	m_InsideVec = {0,0,0};
	VolumeUpdate();
	if (m_BuildInfo->NodeName == L"Center")
	{
		for (auto& obj : SceneManager::GetInstance()->m_CurScene->m_GameObjectList)
		{
		//#여기사람있어요
			if (obj->GetComponent<BoxCollider>() != nullptr && obj.get() != m_pGameObject && obj->GetComponent<MeshRenderer>()->GetMeshInfo()->IDName != L"Smith")
			{
				m_IsCrash = CollisionCheck(obj);
				m_IsTriggerOn = ObjectCheck(obj);
				OnFloor(obj);
				if (m_IsCrash)
				{
					if (!(obj->GetName().find(L"bubble") != wstring::npos))
					{
						MoveReverse();
					}
							
				}
				if (!m_IsOnFloor)
				{
					//if (m_FloorCount <= 0)
					MoveInside();
				}
				if (m_IsTriggerOn)
				{
					if (obj->GetComponent<Interactable>() != nullptr)
					{
						if (find(m_InterActionObjList.begin(), m_InterActionObjList.end(), obj) == m_InterActionObjList.end())
						{
							m_InterActionObjList.emplace_back(obj);
							//m_InterActionObj = obj;
							m_InterActionObjList.back()->GetComponent<MeshRenderer>()->GetMeshInfo()->isInterAct = true;
						}
					}
				}
				else if (!m_IsTriggerOn)
				{
					if (obj->GetComponent<Interactable>() != nullptr && !m_InterActionObjList.empty())
					{
						int objCount = 0;
						for (auto& IAobj : m_InterActionObjList)
						{
							if (IAobj->GetName() == obj->GetName())
							{
								IAobj->GetComponent<MeshRenderer>()->GetMeshInfo()->isInterAct = false;
								m_InterActionObjList.erase(m_InterActionObjList.begin() + objCount);
								return;
							}
							objCount++;
						}
					}
				}
			}
		}
	}
}

void BoxCollider::VolumeUpdate()
{
	if (m_PrevPos != m_Transform->GetWorldPosition())
	{
		Vector3 _dis = m_Transform->GetWorldPosition() - m_PrevPos;
		m_BuildInfo->minX = _dis.x + m_BuildInfo->minX;
		m_BuildInfo->minY = _dis.y + m_BuildInfo->minY;
		m_BuildInfo->minZ = _dis.z + m_BuildInfo->minZ;
		m_BuildInfo->maxX = _dis.x + m_BuildInfo->maxX;
		m_BuildInfo->maxY = _dis.y + m_BuildInfo->maxY;
		m_BuildInfo->maxZ = _dis.z + m_BuildInfo->maxZ;
		if (m_BuildInfo->isCol == true)
		{
			m_BuildInfo->minX2 = _dis.x + m_BuildInfo->minX2;
			m_BuildInfo->minZ2 = _dis.z + m_BuildInfo->minZ2;
			m_BuildInfo->maxX2 = _dis.x + m_BuildInfo->maxX2;
			m_BuildInfo->maxZ2 = _dis.z + m_BuildInfo->maxZ2;
		}
	}
	m_PrevPos = m_Transform->GetWorldPosition();
}

bool BoxCollider::CollisionCheck(shared_ptr<GameObject> otherObj)
{
	if (otherObj->GetComponent<MeshRenderer>() != nullptr && otherObj->GetName() != L"Floor")
	{
		float otherMinX = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->minX;
		float otherMinY = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->minY;
		float otherMinZ = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->minZ;
		float otherMaxX = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->maxX;
		float otherMaxY = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->maxY;
		float otherMaxZ = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->maxZ;


		if (m_BuildInfo->minX < otherMaxX && m_BuildInfo->maxX > otherMinX
			&& m_BuildInfo->minZ < otherMaxZ && m_BuildInfo->maxZ > otherMinZ)
		{
			float dis = 0;
			bool isX = false;
			bool isPlus = false;
			if (m_BuildInfo->minX < otherMaxX)
			{
				if (dis == 0) dis = abs(m_BuildInfo->minX - otherMaxX);
				if (dis >= abs(m_BuildInfo->minX - otherMaxX))
				{
					dis = abs(m_BuildInfo->minX - otherMaxX);
					isX = true;
					isPlus = true;
				}
			}

			if (m_BuildInfo->maxX > otherMinX)
			{
				if (dis == 0) dis = abs(otherMinX - m_BuildInfo->maxX);
				if (dis >= abs(otherMinX - m_BuildInfo->maxX))
				{
					dis = abs(otherMinX - m_BuildInfo->maxX);
					isX = true;
					isPlus = false;
				}
			}

			if (m_BuildInfo->minZ < otherMaxZ)
			{
				if (dis == 0) dis = abs(m_BuildInfo->minZ - otherMaxZ);
				if (dis >= abs(m_BuildInfo->minZ - otherMaxZ))
				{
					dis = abs(m_BuildInfo->minZ - otherMaxZ);
					isX = false;
					isPlus = true;
				}
			}

			if (m_BuildInfo->maxZ > otherMinZ)
			{
				if (dis == 0) dis = abs(otherMinZ - m_BuildInfo->maxZ);
				if (dis >= abs(otherMinZ - m_BuildInfo->maxZ))
				{
					dis = abs(otherMinZ - m_BuildInfo->maxZ);
					isX = false;
					isPlus = false;
				}
			}

			if (isX)
			{
				if (isPlus)
					m_CollisionVec = { dis, 0, 0 };
				else
					m_CollisionVec = { -dis, 0, 0 };
			}
			else
			{
				if (isPlus)
					m_CollisionVec = { 0, 0, dis };
				else
					m_CollisionVec = { 0, 0, -dis };
			}

			return true;
		}

	}
	return false;
}

void BoxCollider::MoveReverse()
{
	Vector3 sumV = m_CollisionVec + (m_InsideVec * 0.1f);
	m_Transform->Translate(m_CollisionVec);
}

void BoxCollider::MoveInside()
{
	m_Transform->Translate(m_InsideVec * 0.1f);
}

bool BoxCollider::ObjectCheck(shared_ptr<GameObject> otherObj)
{
	if (otherObj->GetComponent<MeshRenderer>() != nullptr && otherObj->GetName() != L"Floor")
	{
		float otherMinX = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->minX;
		float otherMinZ = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->minZ;
		float otherMaxX = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->maxX;
		float otherMaxZ = otherObj->GetComponent<MeshRenderer>()->GetMeshInfo()->maxZ;


		if (m_BuildInfo->minX2 < otherMaxX && m_BuildInfo->maxX2 > otherMinX
			&& m_BuildInfo->minZ2 < otherMaxZ && m_BuildInfo->maxZ2 > otherMinZ)
		{
			return true;
		}
	}
	return false;
}

void BoxCollider::OnFloor(shared_ptr<GameObject> otherObj)
{
	if (otherObj->GetComponent<MeshRenderer>() != nullptr && otherObj->GetName() == L"Floor")
	{
		float otherMinX = -1.8f;
		float otherMinZ = -12.f;
		float otherMaxX = 2.7f;
		float otherMaxZ = -5.8f;

		float dis = 0;
		bool isX = false;
		bool isPlus = false;
		if (m_BuildInfo->minX > otherMinX && m_BuildInfo->maxX < otherMaxX - 0.2f && m_BuildInfo->minZ < otherMinZ)
		{
			if (dis == 0) dis = abs(m_BuildInfo->minZ - otherMinZ);
			if (dis >= abs(m_BuildInfo->minZ - otherMinZ))
			{
				dis = abs(m_BuildInfo->minZ - otherMinZ);
				isX = false;
				isPlus = true;
				m_IsOnFloor = false;
			}
		}

		if (m_BuildInfo->minX > otherMinX && m_BuildInfo->maxX > otherMaxX - 0.2f && m_BuildInfo->maxZ < otherMaxZ)
		{
			if (dis == 0) dis = abs(m_BuildInfo->maxX - otherMaxX);
			if (dis >= abs(m_BuildInfo->maxX - otherMaxX))
			{
				dis = abs(m_BuildInfo->maxX - otherMaxX);
				isX = true;
				isPlus = false;
				m_IsOnFloor = false;
			}
		}

		if (m_BuildInfo->minZ > otherMinZ && m_BuildInfo->maxZ < otherMaxZ && m_BuildInfo->maxX > otherMaxX)
		{
			if (dis == 0) dis = abs(otherMaxX - m_BuildInfo->maxX);
			if (dis >= abs(otherMaxX - m_BuildInfo->maxX))
			{
				dis = abs(otherMaxX - m_BuildInfo->maxX);
				isX = true;
				isPlus = false;
				m_IsOnFloor = false;
			}
		}

		if (m_BuildInfo->maxX > otherMaxX + 0.2f && m_BuildInfo->minZ < otherMaxZ)
		{
			if (dis == 0) dis = abs(m_BuildInfo->minZ - otherMaxZ);
			if (dis >= abs(m_BuildInfo->minZ - otherMaxZ))
			{
				dis = abs(m_BuildInfo->minZ - otherMaxZ);
				isX = false;
				isPlus = true;
				m_IsOnFloor = false;
			}
		}

		if (isX)
		{
			if (isPlus)
				m_InsideVec = { dis, 0, 0 };
			else
				m_InsideVec = { -dis, 0, 0 };
		}
		else
		{
			if (isPlus)
				m_InsideVec = { 0, 0, dis };
			else
				m_InsideVec = { 0, 0, -dis };
		}
	}
}
