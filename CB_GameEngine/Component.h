#pragma once
#include "IComponent.h"

class Transform;
class MeshRenderer;
class BoxCollider;

class Component : public IComponent
{
public:
	Component();
	virtual ~Component() {};
	virtual void Init() override;
	shared_ptr<Transform> m_Transform;
	shared_ptr<MeshRenderer> m_MeshRenderer;
	shared_ptr<BoxCollider> m_Physics;
};

