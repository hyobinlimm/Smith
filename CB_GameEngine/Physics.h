#pragma once

class Transform;
class GameObject;

class Physics : public Component
{
public:
	Physics() {};
	virtual ~Physics() {};

	virtual void Start() override;
	virtual void Update() override;
	virtual void Awake() override;

	void SetInfo(float width, float depth, float height);
	void SetCollapsedObject(shared_ptr<GameObject> target);
	void UnSetCollpasedObject(shared_ptr<GameObject> target);
	bool CollideCheck(shared_ptr<GameObject> target);


public:
	float m_Width;
	float m_Depth;
	float m_Height;
	float m_Scale = 1;
	float m_Left;
	float m_Right;
	float m_Top;
	float m_Bottom;
	float m_Up;
	float m_Down;

	vector<shared_ptr<GameObject>>	m_CollapsedObjects;

private:
	shared_ptr<Transform> m_Transform;
	Vector3 Position;

};

