#include "CCPch.h"
#include "GameTimer.h"
#include "SceneManager.h"
#include "AnimState.h"

void AnimState::Awake()
{
}

void AnimState::Start()
{

}

void AnimState::Update()
{
	m_Timer += GameTimer::GetInstance()->DeltaTime();
	m_Count++;
	float handtick= 1;

	if (m_Timer > 1.3f)
	{
		if (m_HandOverTimer)
		{
			m_HandOverTimer = false;
			m_Timer = 0;
			SetIdle();
		}
	}


}

void AnimState::HandOverStart()
{
	m_HandOverTimer = true;
	m_Timer = 0;
	SceneManager::GetInstance()->m_HimeEngine->SetAniStateType(L"Smith", 0);
}

void AnimState::WalkStart()
{
	SceneManager::GetInstance()->m_HimeEngine->SetAniStateType(L"Smith", 2);
}

void AnimState::SetIdle()
{
	SceneManager::GetInstance()->m_HimeEngine->SetAniStateType(L"Smith", 1);
}
