#include "CBpch.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include <algorithm>
#include "Physics.h"


void Physics::Start()
{
	m_Transform = m_pGameObject->GetComponent<Transform>();

}

void Physics::Update()
{
	Position = m_Transform->GetWorldPosition();
	m_Left = Position.x - m_Width / 2;
	m_Right = Position.x + m_Width / 2;
	m_Up = Position.z + m_Height / 2;
	m_Down = Position.z - m_Height / 2;
	m_Top = Position.y + m_Depth / 2;
	m_Bottom = Position.y - m_Depth / 2;
}

void Physics::Awake()
{

}

void Physics::SetInfo(float width, float depth, float height)
{
	m_Width = width;
	m_Height = height;
	m_Depth = depth;
}

void Physics::SetCollapsedObject(shared_ptr<GameObject> target)
{
	bool Isfound = none_of(m_CollapsedObjects.begin(), m_CollapsedObjects.end(), [&](shared_ptr<GameObject> obj) {return obj == target; });
	if (Isfound)
	{
		TRACE("충돌해서 추가해줬습니당 ㅎㅎ\n");
		m_CollapsedObjects.emplace_back(target);
	}

}



void Physics::UnSetCollpasedObject(shared_ptr<GameObject> target)
{
	m_CollapsedObjects.erase(remove_if(m_CollapsedObjects.begin(), m_CollapsedObjects.end(), [&](shared_ptr<GameObject> obj) {return obj == target; }), m_CollapsedObjects.end());
}

bool Physics::CollideCheck(shared_ptr<GameObject> target)
{
	shared_ptr<Physics> _targetPhysics = target->GetComponent<Physics>();
	if (((m_Left >= _targetPhysics->m_Right) || (m_Right <= _targetPhysics->m_Left)) &&
		((m_Top >= _targetPhysics->m_Bottom) || (m_Bottom <= _targetPhysics->m_Top)) &&
		((m_Up >= _targetPhysics->m_Down) || (m_Down <= _targetPhysics->m_Up)))
	{
		return true;
	}
	return false;
}

