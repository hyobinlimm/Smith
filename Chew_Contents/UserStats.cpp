#include "CCPch.h"
#include "Component.h"
#include "GameTimer.h"
#include "UserStats.h"

UserStats::UserStats()
{

}

UserStats::~UserStats()
{

}

void UserStats::Awake()
{
	m_Timer = GameTimer::GetInstance();
}
