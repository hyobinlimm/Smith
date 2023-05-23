#pragma once

#include "Object.h"
#include "GameObject.h"

/// <summary>
/// 2022.08.01
/// ��� ������Ʈ���� �������־�� �� �͵��� ������ ��.
/// ������Ʈ�� �߰����Ѿ� �� ���?
/// �̵��� ����� Transform
/// ���� �浹�� ����� Physics
/// �׸��⸦ ����� Renderer 
/// �켱 ���� �� ����� �߰��غ���.
/// </summary>

class IComponent : public Object
{
public:
	IComponent();
	virtual ~IComponent() {};

	virtual void Awake() abstract;
	virtual void Start() abstract;
	virtual void Update() abstract;
	virtual void Init() abstract;
	//virtual void Render() abstract;

public:
	template <typename T>
	void AddComponent();

	template <typename T>
	shared_ptr<T> GetComponent();

public:
	wstring m_Name;
	GameObject* m_pGameObject;
};

template <typename T>
shared_ptr<T> IComponent::GetComponent()
{
	if (m_pGameObject != nullptr)
	{
		return m_pGameObject->template GetComponent<T>();
	}

	return nullptr;
}

template <typename T>
void IComponent::AddComponent()
{
	if (m_pGameObject != nullptr)
	{
		return m_pGameObject->template AddComponent<T>();
	}

	return nullptr;
}