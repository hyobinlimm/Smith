#include "GEpch.h"
#include "GameObject.h"
#include "IComponent.h"

GameObject::GameObject(wstring name)
	: m_Name(name), m_ChildObject(nullptr), m_ParentObject(nullptr)
{

}

void GameObject::Start()
{
	for (shared_ptr<IComponent> co : m_pComponents)
	{
		co->Start();
	}
}

void GameObject::Awake()
{
	for (shared_ptr<IComponent> co : m_pComponents)
	{
		co->Awake();
	}
}

void GameObject::Update()
{
	for (shared_ptr<IComponent> co : m_pComponents)
	{
		co->Update();
	}
}

//void GameObject::Render()
//{
//	for (shared_ptr<IComponent> co : m_pComponents)
//	{
//		co->Render();
//	}
//}
