#include "CBpch.h"
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"

Component::Component()
{
}

void Component::Init()
{
	if (m_pGameObject->GetComponent<Transform>() != nullptr)
	{
		m_Transform = m_pGameObject->GetComponent<Transform>();
	}
	if (m_pGameObject->GetComponent<MeshRenderer>() != nullptr)
	{
		m_MeshRenderer = m_pGameObject->GetComponent<MeshRenderer>();
	}
	if (m_pGameObject->GetComponent<BoxCollider>() != nullptr)
	{
		m_Physics = m_pGameObject->GetComponent<BoxCollider>();
	}
}
