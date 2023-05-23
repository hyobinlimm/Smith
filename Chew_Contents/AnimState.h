#pragma once
#include <Component.h>
class AnimState :
	public Component
{
public:
	virtual void Awake() override;


	virtual void Start() override;


	virtual void Update() override;

public:
	void HandOverStart();
	void WalkStart();
	void SetIdle();



private:
	bool m_walkTimer = false;
	bool m_HandOverTimer = false;
	float m_Timer = 0;
	int m_Count = 0;
};

