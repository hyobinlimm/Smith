#include "CCPch.h"
#include "GameScene.h"
#include "HimeRendererStructs.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "WeaponManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "TextRenderer.h"
#include "UIRenderer.h"
#include "Transform.h"
#include "GameTimer.h"
#include "MeshRenderer.h"
#include "Movement.h"
#include "ObjectDataTable.h"
#include "UIManager.h"
#include "InputManager.h"
#include "QuestManager.h"
#include "UI.h"
#include "Interactable.h"
#include "BoxCollider.h"

GameScene::GameScene(wstring sceneName)
	: Scene(sceneName)
{

	shared_ptr<TextData> fpsData = make_shared<TextData>();
	fpsData->IDName = L"FPS";
	fpsData->text = L"FPS : %0.2f";
	fpsData->color = { 1.f, 1.f, 1.f, 1.f };
	fpsData->FontSize = 2.f;
	fpsData->x = 50;
	fpsData->y = 40;
	fpsData->value = 0;
	fpsData->isDraw = false;

	shared_ptr<TextData> mspfData = make_shared<TextData>();
	mspfData->IDName = L"Frame Time";
	mspfData->text = L"Frame Time : %0.2f";
	mspfData->color = { 1.f, 1.f, 1.f, 1.f };
	mspfData->FontSize = 2.f;
	mspfData->x = 50;
	mspfData->y = 70;
	mspfData->value = 0;
	mspfData->isDraw = false;

	shared_ptr<TextData> screenData = make_shared<TextData>();
	screenData->IDName = L"Screen";
	screenData->text = L"Width  Height : %0.2f  %0.2f";
	screenData->color = { 1.f, 1.f, 1.f, 1.f };
	screenData->FontSize = 2.f;
	screenData->x = 50;
	screenData->y = 130;
	screenData->value = 0;
	screenData->value2 = 0;
	screenData->isDraw = false;

	shared_ptr<TextData> ObjAmountData = make_shared<TextData>();
	ObjAmountData->IDName = L"Object";
	ObjAmountData->text = L"Object Amount : %f";
	ObjAmountData->color = { 1.f, 1.f, 1.f, 1.f };
	ObjAmountData->FontSize = 2.f;
	ObjAmountData->x = 50;
	ObjAmountData->y = 250;
	ObjAmountData->value = 0;
	ObjAmountData->isDraw = false;


	shared_ptr<TextData> sceneData = make_shared<TextData>();
	sceneData->IDName = L"Scene";
	sceneData->text = L"Scene Name : %ls";
	sceneData->color = { 1.f, 1.f, 1.f, 1.f };
	sceneData->FontSize = 2.f;
	sceneData->x = 50;
	sceneData->y = 160;
	sceneData->textValue = m_Name;
	sceneData->isDraw = false;

	shared_ptr<TextData> RenderObjAmountData = make_shared<TextData>();
	RenderObjAmountData->IDName = L"RenderObject";
	RenderObjAmountData->text = L"RenderObject Amount : %f";
	RenderObjAmountData->color = { 1.f, 1.f, 1.f, 1.f };
	RenderObjAmountData->FontSize = 2.f;
	RenderObjAmountData->x = 50;
	RenderObjAmountData->y = 850;
	RenderObjAmountData->value = 0;
	RenderObjAmountData->isDraw = false;

	sceneTime = make_shared<TextData>();
	sceneTime->IDName = L"Scene";
	sceneTime->text = L"Scene Timer : %f\nScene Date : %f";
	sceneTime->color = { 1.f, 1.f, 1.f, 1.f };
	sceneTime->FontSize = 2.f;
	sceneTime->x = 50;
	sceneTime->y = 900;
	sceneTime->value = 0;
	sceneTime->value2 = 0;
	sceneTime->isDraw = false;

	m_stringList.emplace_back(fpsData);
	m_stringList.emplace_back(mspfData);
	m_stringList.emplace_back(screenData);
	m_stringList.emplace_back(sceneData);
	m_stringList.emplace_back(ObjAmountData);
	m_stringList.emplace_back(RenderObjAmountData);

	m_stringList.emplace_back(sceneTime);
}

void GameScene::Awake()
{
	SoundManager::GetInstance()->BgmSetting("GameBGM");
	SoundManager::GetInstance()->BgmSoundControl(0.4f); 
	this->m_HimeEngine = SceneManager::GetInstance()->m_HimeEngine;
	CreateUI();
	WeaponManager::GetInstance()->Initiallize();
	QuestManager::GetInstance()->Initialize();

	for (auto& obj : m_GameObjectList)
	{
		if (obj != nullptr)
		{
			obj->Awake();
			if (obj->GetComponent<MeshRenderer>() != nullptr)
			{
				m_RenderGameObjectList.emplace_back(obj);
			}
			if (obj->GetComponent<UIRenderer>() != nullptr)
			{
				if (obj->GetComponent<UIRenderer>()->GetUIData()->isDraw == true)
				{
					m_RenderGameObjectList.emplace_back(obj);
				}
			}
			if (obj->GetComponent<Camera>() != nullptr)
			{
				m_HimeEngine->CreateStringBuffer(obj->GetComponent<TextRenderer>()->GetTextData().get());
			}

			if (obj->GetComponent<UIRenderer>() != nullptr)
			{

				m_TexList.emplace_back(obj->GetComponent<UIRenderer>()->GetUIData().get());
			}
		}
	}

	for (auto& text : m_stringList)
	{
		m_HimeEngine->CreateStringBuffer(text.get());
	}
}

void GameScene::Start()
{
	for (auto& obj : m_GameObjectList)
	{
		if (obj->GetName() == L"MainUI")
		{
			for (auto& _UI : obj->GetComponent<UI>()->GetUIList())
			{
				if (_UI->GetUIData()->Type == L"Main")
				{
					_UI->GetUIData()->isDraw = true;
				}
				if (_UI->GetUIData()->Type == L"Day" && _UI->GetUIData()->Data == to_wstring(m_SceneDate))
				{
					_UI->GetUIData()->isDraw = true;
				}
			}
		}

		if (obj != nullptr)
		{
			obj->Start();
		}
	}

	m_HimeEngine->SetUITextureData(m_TexList);
}

void GameScene::Update()
{
	static float _frametimer = 0;
	_frametimer += GameTimer::GetInstance()->DeltaTime();
	// 이펙트 스프라이트 테스트
	for (auto& ui : m_TexList)
	{
		if (ui->Type == L"Effect")
		{
			float a = _frametimer * 10;

			ui->frame = (int)a % 7;
		}
	}

	for (auto& obj : m_GameObjectList)
	{
		if (obj != nullptr)
		{
			if (obj->GetName() == L"Smith")
			{
				CharacterPosData->value = obj->GetComponent<Transform>()->GetWorldPosition().x;
				CharacterPosData->value2 = obj->GetComponent<Transform>()->GetWorldPosition().y;
				CharacterPosData->value3 = obj->GetComponent<Transform>()->GetWorldPosition().z;
				CharacterRotData->value = obj->GetComponent<Transform>()->GetWorldRotate().x * Rad2Deg;
				CharacterRotData->value2 = obj->GetComponent<Transform>()->GetWorldRotate().y * Rad2Deg;
				CharacterRotData->value3 = obj->GetComponent<Transform>()->GetWorldRotate().z * Rad2Deg;
			}
			if (obj->GetComponent<MeshRenderer>() != nullptr)
			{
				m_HimeEngine->Update(GameTimer::GetInstance()->DeltaTime(), obj->GetComponent<MeshRenderer>()->GetMeshInfo());
			}
			if (obj->GetComponent<Camera>() != nullptr)
			{
				m_HimeEngine->CameraUpdate(obj->GetComponent<Camera>().get()->GetCameraInfo().get());
			}
			obj->Update();
		}
	}
	DebugTextOnOff();
}

void GameScene::Render()
{
	for (auto& obj : m_GameObjectList)
	{
		if (obj != nullptr)
		{
			if (obj->GetComponent<Renderer>() != nullptr)
			{
				obj->GetComponent<Renderer>()->Render();
			}
		}
	}
}

void GameScene::LoadObject(vector<MeshBuildInfo*> MeshInfoList, shared_ptr<ObjectDataTable> ObjData)
{
	for (auto& _MeshInfo : MeshInfoList)
	{
		shared_ptr<GameObject> _NewGameObject = make_shared<GameObject>(_MeshInfo->IDName);
		_NewGameObject->AddComponent<Transform>();
		_NewGameObject->AddComponent<MeshRenderer>();
		//_NewGameObject->AddComponent<BoxCollider>();

		if (_MeshInfo->IDName == L"Smith")
		{
			_NewGameObject->AddComponent<Movement>();
			_NewGameObject->GetComponent<Movement>()->SetMoveSpeed(0.1f);
			_NewGameObject->GetComponent<Movement>()->SetRotateSpeed(10.0f);
		}
		for (size_t i = 0; i < ObjData->m_DataCount; i++)
		{
			if (_MeshInfo->IDName == ObjData->m_Name[i])
			{
				_NewGameObject->GetComponent<Transform>()->SetLocalPosition({ _MeshInfo->Pos.x, _MeshInfo->Pos.y , _MeshInfo->Pos.z });
				_NewGameObject->GetComponent<Transform>()->SetLocalRotate({ _MeshInfo->Rot.x, _MeshInfo->Rot.y , _MeshInfo->Rot.z });
				_NewGameObject->GetComponent<Transform>()->SetLocalScale({ _MeshInfo->Scale.x, _MeshInfo->Scale.y , _MeshInfo->Scale.z });
				_MeshInfo->isDraw = true;
			}
			_NewGameObject->GetComponent<MeshRenderer>()->SetMeshInfo(_MeshInfo);
			if (_NewGameObject->GetName() == L"Smith")
			{
				if (CharacterPosData == nullptr)
				{
					CharacterPosData = make_shared<TextData>();
					CharacterPosData->IDName = L"Character";
					CharacterPosData->text = L"Character Pos : %0.2f, %0.2f, %0.2f";
					CharacterPosData->color = { 1.f, 1.f, 1.f, 1.f };
					CharacterPosData->FontSize = 2.f;
					CharacterPosData->x = 50;
					CharacterPosData->y = 190;
					CharacterPosData->isDraw = false;
					CharacterPosData->value = _MeshInfo->Pos.x;
					CharacterPosData->value2 = _MeshInfo->Pos.y;
					CharacterPosData->value3 = _MeshInfo->Pos.z;
					m_stringList.emplace_back(CharacterPosData);
				}
				if (CharacterRotData == nullptr)
				{
					CharacterRotData = make_shared<TextData>();
					CharacterRotData->IDName = L"Character";
					CharacterRotData->text = L"Character Rot : %0.2f, %0.2f, %0.2f";
					CharacterRotData->color = { 1.f, 1.f, 1.f, 1.f };
					CharacterRotData->FontSize = 2.f;
					CharacterRotData->x = 50;
					CharacterRotData->y = 220;
					CharacterRotData->isDraw = false;
					CharacterRotData->value = _MeshInfo->Rot.x;
					CharacterRotData->value2 = _MeshInfo->Rot.y;
					CharacterRotData->value3 = _MeshInfo->Rot.z;
					m_stringList.emplace_back(CharacterRotData);
				}
			}
			else if (_NewGameObject->GetName() == L"Showcase")
			{
				_NewGameObject->AddComponent<Interactable>();
				_NewGameObject->GetComponent<Interactable>()->SetSequence(0);
			}
			else if (_NewGameObject->GetName() == L"Firepot")
			{
				_NewGameObject->AddComponent<Interactable>();
				_NewGameObject->GetComponent<Interactable>()->SetSequence(2);
			}
			else if (_NewGameObject->GetName() == L"Enchant")
			{
				_NewGameObject->AddComponent<Interactable>();
				_NewGameObject->GetComponent<Interactable>()->SetSequence(5);
			}
			else if (_NewGameObject->GetName() == L"Anvil")
			{
				_NewGameObject->AddComponent<Interactable>();
				_NewGameObject->GetComponent<Interactable>()->SetSequence(4);
			}
			else if (_NewGameObject->GetName() == L"Frame")
			{
				_NewGameObject->AddComponent<Interactable>();
				_NewGameObject->GetComponent<Interactable>()->SetSequence(3);
			}
			else if (_NewGameObject->GetName() == L"Counter")
			{
				_NewGameObject->AddComponent<Interactable>();
				_NewGameObject->GetComponent<Interactable>()->SetSequence(7);
			}

			m_GameObjectList.emplace_back(_NewGameObject);
		}
	}
}

void GameScene::CreateUI()
{
	for (auto& _UI : UIManager::GetInstance()->m_MainUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_QuestUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);

		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_MonsterUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_ReviewUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_WeaponUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);

		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_MaterialUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);

		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_PouringUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);

		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_MeltingUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);

		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_HammeringUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_EnchantHitUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_EnchantSelectUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_HandleUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}
}

void GameScene::DebugTextOnOff()
{
	if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F12) == KeyState::Down)
	{
		for (auto& text : m_stringList)
		{
			text->isDraw = !text->isDraw;
		}
		for (auto& obj : m_GameObjectList)
		{
			if (obj->GetComponent<TextRenderer>() != nullptr)
			{
				obj->GetComponent<TextRenderer>()->GetTextData()->isDraw = !obj->GetComponent<TextRenderer>()->GetTextData()->isDraw;
			}
		}
	}
}
