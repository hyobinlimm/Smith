#include "CCPch.h"
#include "GameTimer.h"
#include "HimeEngine.h"
#include "GameObject.h"
#include "HimeRendererStructs.h"
#include "UIManager.h"
#include "UIRenderer.h"
#include "UI.h"
#include "UIPage.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ControllerEnum.h"
#include "TitleScene.h"
#include "SoundManager.h"

TitleScene::TitleScene(wstring sceneName)
	:Scene(sceneName)
{

}
void TitleScene::Awake()
{
	 m_Scene = SceneManager::GetInstance();
	SoundManager::GetInstance()->BgmSetting("TitleBGM");
	SoundManager::GetInstance()->BgmSoundControl(0.4f);

	this->m_HimeEngine = SceneManager::GetInstance()->m_HimeEngine;
	CreateUI();

	shared_ptr<GameObject> _TitleUI = make_shared<GameObject>(L"TitleUI");
	_TitleUI->AddComponent<UI>();
	_TitleUI->AddComponent<UIPage>();
	m_GameObjectList.emplace_back(_TitleUI);

	shared_ptr<GameObject> _CreditUI = make_shared<GameObject>(L"CreditUI");
	_CreditUI->AddComponent<UI>();
	m_GameObjectList.emplace_back(_CreditUI);

	for (auto& obj : m_GameObjectList)
	{

		if (obj != nullptr)
		{
			obj->Awake();
			if (obj->GetComponent<UIRenderer>() != nullptr)
			{
				m_TexList.emplace_back(obj->GetComponent<UIRenderer>()->GetUIData().get());
			}
		}
	}
	m_HimeEngine->SetFireEffect(false);
}
void TitleScene::Start()
{
	for (auto& obj : m_GameObjectList)
	{
		if (obj != nullptr)
		{
			obj->Start();
		}

		if (obj->GetName() == L"TitleUI")
		{
			obj->GetComponent<UI>()->OpenUI();
			obj->GetComponent<UIPage>()->UpdateCategory();
			obj->GetComponent<UIPage>()->UpdatePage();
		}
		//¹¹¿©
	}

	m_HimeEngine->SetUITextureData(m_TexList);

}
void TitleScene::Update()
{
	for (auto& obj : m_GameObjectList)
	{
		if (obj != nullptr)
		{
			obj->Update();
		}
	}
	m_HimeEngine->Update(GameTimer::GetInstance()->DeltaTime());

	KeyUpdate();

}
void TitleScene::KeyUpdate()
{
	if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::SpaceBar) == KeyState::Down)
	{
		for (auto& obj : m_GameObjectList)
		{
			if (obj->GetName() == L"TitleUI")
			{
				SoundManager::GetInstance()->SFXPlay("Menu_Select", 4);
				switch (obj->GetComponent<UIPage>()->GetIndex())
				{
				case 1:
					obj->GetComponent<UI>()->CloseUI();
					SceneManager::GetInstance()->LoadScene(L"TestScene");
				
					SceneManager::GetInstance()->m_CurScene->Awake();
					SceneManager::GetInstance()->m_CurScene->Start();
					break;
				case 2:
					for (auto& _UI : m_GameObjectList)
					{
						if (_UI->GetName() == L"CreditUI")
						{
							_UI->GetComponent<UI>()->SwitchUI();
							m_IsCreditOn = !m_IsCreditOn;
						}
					}
					break;
				case 3:
					exit(0);
					break;
				default:
					break;
				}

			}
		}
	}
	else if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::UpArrow) == KeyState::Down)
	{
		SoundManager::GetInstance()->SFXPlay("Menu_Move", 0);
		if (m_IsCreditOn) return;
		for (auto& obj : m_GameObjectList)
		{
			if (obj->GetName() == L"TitleUI")
			{

				obj->GetComponent<UIPage>()->Change(UP);

			}
		}
	}
	else if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::DownArrow) == KeyState::Down)
	{
		SoundManager::GetInstance()->SFXPlay("Menu_Move", 0);
		if (m_IsCreditOn) return;
		for (auto& obj : m_GameObjectList)
		{
			if (obj->GetName() == L"TitleUI")
			{
				obj->GetComponent<UIPage>()->Change(DOWN);
			}
		}
	}
}
void TitleScene::Render()
{

}
void TitleScene::LoadObject(vector<MeshBuildInfo*> MeshInfoList, shared_ptr<ObjectDataTable> ObjData)
{

}
void TitleScene::CreateUI()
{
	for (auto& _UI : UIManager::GetInstance()->m_TitleUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);
		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_CreditUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);
		m_GameObjectList.emplace_back(_NewUI);
	}
}