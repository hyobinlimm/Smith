#pragma once

class GameTimer
{
public:
	GameTimer();

	~GameTimer() = default;

	GameTimer(const GameTimer& other) = delete;

	GameTimer& operator= (const GameTimer& other) = delete;

	GameTimer(GameTimer&& other) = delete;

public:
	static shared_ptr<GameTimer> GetInstance();

	float TotalTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

	bool SetTimer(float time);

private:
	static shared_ptr<GameTimer> g_TimerInstance;

	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;
	__int64 m_SetTime;


	bool m_Stopped;

};

