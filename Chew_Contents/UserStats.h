#pragma once

class GameTimer;

class UserStats :
	public Component
{
public:
	UserStats();
	~UserStats();

	void Start() {}
	void Update() {}
	void Awake();
private:

	shared_ptr<GameTimer> m_Timer;
	int m_Fame;
	float m_Time;


};

