#pragma once

/// <summary>
/// 2022.08.01
/// 게임 오브젝트는 무엇을 가지고 있어야하나.
/// 오브젝트에 따라 필요한 기능들을 가진 컴포넌트들을 추가해보자.
/// </summary>

class IComponent;

class GameObject
{
public:
	GameObject(wstring name);
	virtual ~GameObject() {};

public:
	void Start();
	void Awake();
	void Update();
	//void Render();

	wstring GetName() const { return m_Name; }
	void SetName(wstring val) { m_Name = val; }

	void AddChildObject(shared_ptr<GameObject> childObject) { m_ChildObject = childObject; }
	void SetParentObject(shared_ptr<GameObject> parentObject) { m_ParentObject = parentObject; }


	shared_ptr<GameObject> GetParentObject() { return m_ParentObject; }
	
private:
	vector<shared_ptr<GameObject>> m_pChildObjects;
	vector<shared_ptr<IComponent>> m_pComponents;
	wstring m_Name;
	shared_ptr<GameObject> m_ChildObject;
	shared_ptr<GameObject> m_ParentObject;

	// 컴포넌트를 추가 또는 획득
public:
	template <typename T>
	void AddComponent();

	template <typename T>
	shared_ptr<T> GetComponent();
};

template <typename T>
void GameObject::AddComponent()
{
	shared_ptr<IComponent> _component = make_shared<T>();

	_component->m_pGameObject = this;
	_component->Init();

	m_pComponents.push_back(_component);
}

template <typename T>
shared_ptr<T> GameObject::GetComponent()
{
	for (size_t i = 0; i < m_pComponents.size(); i++)
	{

		const type_info& temp1 = typeid(T);
		const type_info& temp2 = typeid(*(m_pComponents[i].get()));

		//if (_m == nullptr)
		//{
		//	//assert(false);
		//	return nullptr;
		//}
		//else
		//{
		//	return shared_ptr<T>(_m);
		//}

		if (temp1 == temp2)
		{
			shared_ptr<T> _component = dynamic_pointer_cast<T>(m_pComponents[i]);
			return _component;
		}
			/*shared_ptr<T> _component = dynamic_pointer_cast<T>(m_pComponents[i]);

			if (_component != nullptr)
				return _component;*/
	}

	return nullptr;
}
