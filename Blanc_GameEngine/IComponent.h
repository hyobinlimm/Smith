#pragma once

#include "Object.h"
#include "GameObject.h"

/// <summary>
/// 2022.08.01
/// 모든 컴포넌트들이 가지고있어야 할 것들을 정의할 곳.
/// 컴포넌트로 추가시켜야 할 기능?
/// 이동을 담당할 Transform
/// 물리 충돌을 담당할 Physics
/// 그리기를 담당할 Renderer 
/// 우선 위의 세 기능을 추가해보자.
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