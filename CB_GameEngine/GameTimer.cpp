#include "CBpch.h"
#include "GameTimer.h"

shared_ptr<GameTimer> GameTimer::g_TimerInstance = nullptr;

GameTimer::GameTimer()
	: m_SecondsPerCount(0.0), m_DeltaTime(-1.0), 
	m_BaseTime(0), m_PausedTime(0), m_PrevTime(0), m_CurrTime(0), m_Stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0f / (double)countsPerSec;
}

std::shared_ptr<GameTimer> GameTimer::GetInstance()
{
	if (g_TimerInstance == nullptr)
		g_TimerInstance = make_shared<GameTimer>();

	return g_TimerInstance;
}

float GameTimer::TotalTime() const
{
	if (m_Stopped)
	{
		return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}

	else
	{
		return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
}

float GameTimer::DeltaTime() const
{
	return (float)m_DeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_Stopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_Stopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}
}

void GameTimer::Stop()
{
	if (!m_Stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_Stopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	// 이번 프레임의 시간을 얻는다.
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	// 이 시간과 이전 프레임의 시간의 차이를 구한다.
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

	// 다음 프레임을 준비한다.
	m_PrevTime = m_CurrTime;

	// 음수가 되지 않게 한다.
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}

bool GameTimer::SetTimer(float time)
{
	m_SetTime = m_CurrTime;
	if (TotalTime() - m_CurrTime > m_SetTime)
	{
		return true;
	}
	return false;
}
