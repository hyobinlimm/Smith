#include "CCpch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Movement.h"
#include "Controller.h"
#include "Renderer.h"
#include "UserStats.h"
#include "HimeEngine.h"
#include "Camera.h"
#include "UIRenderer.h"
#include "CameraController.h"
#include "ObjectDataTable.h"
#include "UIRenderer.h"
#include "TextRenderer.h"
#include "GameTimer.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "UIManager.h"
#include "InputManager.h"
#include "VirtualKey.h"
#include "KeyState.h"
#include "WeaponManager.h"
#include "QuestManager.h"
#include "QuestDataTable.h"
#include "WeaponQuest.h"
#include "UI.h"
#include "SelectMinigame.h"
#include "HitMinigame.h"
#include "UIPage.h"
#include "QuestUI.h"
#include "Interactable.h"
#include "AnimState.h"
#include "BoxCollider.h"
#include "Environment.h"
#include "TestScene.h"
#include "CountDown.h"
#include "AIController.h"

TestScene::TestScene(wstring sceneName)
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

	ObjAmountData = make_shared<TextData>();
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

	RenderObjAmountData = make_shared<TextData>();
	RenderObjAmountData->IDName = L"RenderObject";
	RenderObjAmountData->text = L"RenderObject Amount : %f";
	RenderObjAmountData->color = { 1.f, 1.f, 1.f, 1.f };
	RenderObjAmountData->FontSize = 2.f;
	RenderObjAmountData->x = 50;
	RenderObjAmountData->y = 850;
	RenderObjAmountData->value = 0;
	RenderObjAmountData->isDraw = false;



	m_stringList.emplace_back(fpsData);
	m_stringList.emplace_back(mspfData);
	m_stringList.emplace_back(screenData);
	m_stringList.emplace_back(ObjAmountData);
	m_stringList.emplace_back(RenderObjAmountData);


	m_Warrior = make_shared<GameObject>(L"Warrior");
	m_Warrior->AddComponent<AIController>();
	m_GameObjectList.emplace_back(m_Warrior);
}

void TestScene::Awake()
{
	SoundManager::GetInstance()->BgmSetting("GameBGM");
	SoundManager::GetInstance()->BgmSoundControl(0.4f);

	this->m_HimeEngine = SceneManager::GetInstance()->m_HimeEngine;
	m_HimeEngine->SetFireEffect(true);
	
	CreateUI();
	shared_ptr<GameObject> _User = make_shared<GameObject>(L"Controller");
	_User->AddComponent<Controller>();
	_User->AddComponent<WeaponQuest>();
	m_GameObjectList.emplace_back(_User);

	QuestManager::GetInstance()->Initialize();
	// 무기정보들 json으로 잘 읽어와지는지 테스트
	WeaponManager::GetInstance()->Initiallize();

	shared_ptr<GameObject> _MainUI = make_shared<GameObject>(L"MainUI");
	_MainUI->AddComponent<UI>();
	m_GameObjectList.emplace_back(_MainUI);


	shared_ptr<GameObject> _WeaponUI = make_shared<GameObject>(L"WeaponUI");
	_WeaponUI->AddComponent<UI>();
	_WeaponUI->AddComponent<UIPage>();
	_WeaponUI->AddComponent<SelectMinigame>();
	m_GameObjectList.emplace_back(_WeaponUI);

	shared_ptr<GameObject> _EnchantUI = make_shared<GameObject>(L"EnchantSelectUI");
	_EnchantUI->AddComponent<UI>();
	_EnchantUI->AddComponent<UIPage>();
	_EnchantUI->AddComponent<SelectMinigame>();
	m_GameObjectList.emplace_back(_EnchantUI);

	shared_ptr<GameObject> _HandleUI = make_shared<GameObject>(L"HandleUI");
	_HandleUI->AddComponent<UI>();
	_HandleUI->AddComponent<UIPage>();
	_HandleUI->AddComponent<SelectMinigame>();
	m_GameObjectList.emplace_back(_HandleUI);

	shared_ptr<GameObject> _MaterialUI = make_shared<GameObject>(L"MaterialUI");
	_MaterialUI->AddComponent<UI>();
	_MaterialUI->AddComponent<UIPage>();
	_MaterialUI->AddComponent<SelectMinigame>();
	m_GameObjectList.emplace_back(_MaterialUI);

	shared_ptr<GameObject> _MonsterUI = make_shared<GameObject>(L"MonsterUI");
	_MonsterUI->AddComponent<UI>();
	_MonsterUI->AddComponent<UIPage>();
	m_GameObjectList.emplace_back(_MonsterUI);

	shared_ptr<GameObject> _QuestUI = make_shared<GameObject>(L"QuestUI");
	_QuestUI->AddComponent<UI>();
	_QuestUI->AddComponent<QuestUI>();
	m_GameObjectList.emplace_back(_QuestUI);

	shared_ptr<GameObject> _ReviewUI = make_shared<GameObject>(L"ReviewUI");
	_ReviewUI->AddComponent<UI>();
	m_GameObjectList.emplace_back(_ReviewUI);

	shared_ptr<GameObject> _EnchantHitUI = make_shared<GameObject>(L"EnchantHitUI");
	_EnchantHitUI->AddComponent<UI>();
	_EnchantHitUI->AddComponent<HitMinigame>();
	_EnchantHitUI->AddComponent<CountDown>();
	m_GameObjectList.emplace_back(_EnchantHitUI);

	shared_ptr<GameObject> _HammeringUI = make_shared<GameObject>(L"HammeringUI");
	_HammeringUI->AddComponent<UI>();
	_HammeringUI->AddComponent<HitMinigame>();
	_HammeringUI->AddComponent<CountDown>();
	m_GameObjectList.emplace_back(_HammeringUI);

	shared_ptr<GameObject> _MeltingUI = make_shared<GameObject>(L"MeltingUI");
	_MeltingUI->AddComponent<UI>();
	_MeltingUI->AddComponent<HitMinigame>();
	_MeltingUI->AddComponent<CountDown>();
	m_GameObjectList.emplace_back(_MeltingUI);

	shared_ptr<GameObject> _PouringUI = make_shared<GameObject>(L"PouringUI");
	_PouringUI->AddComponent<UI>();
	_PouringUI->AddComponent<HitMinigame>();
	_PouringUI->AddComponent<CountDown>();
	m_GameObjectList.emplace_back(_PouringUI);

	shared_ptr<GameObject> _Environment = make_shared<GameObject>(L"Environment");
	_Environment->AddComponent<Environment>();
	m_GameObjectList.emplace_back(_Environment);


	shared_ptr<GameObject> _EndingUI = make_shared<GameObject>(L"EndingUI");
	_EndingUI->AddComponent<UI>();
	m_GameObjectList.emplace_back(_EndingUI);

	SoundManager::GetInstance()->BgmSetting("_CountDownUI");

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

void TestScene::Start()
{
	m_Warrior->GetComponent<AIController>()->MakeWarrior();
	for (auto& obj : m_GameObjectList)
	{
		if (obj != nullptr)
		{
			if (obj->GetName() == L"Controller")
			{
				obj->Start();
			}
			else
			{
				obj->Start();
			}
			
		}
		

	}

	m_HimeEngine->SetUITextureData(m_TexList);


	for (auto& ui : m_TexList)
	{
		if (ui->Type == L"Effect")
		{
			ui->width = 16128;
			ui->height = 1296;
		}
		else if (ui->Type == L"Animation")
		{
			ui->width = 9920;
			ui->height = 620;
		}
	}
}

void TestScene::Update()
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
		else if (ui->Type == L"Animation")
		{
			float a = _frametimer * 10;

			ui->frame = (int)a % 16;
		}
	}


	BgmChange();
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

			if (obj->GetName().find(L"bubble")!=wstring::npos)
			{
				obj->GetComponent<Transform>()->SetLocalPosition({ CharacterPosData->value, CharacterPosData->value2 + 3.f , CharacterPosData->value3 });
			}
			/*		ObjAmountData->value = m_GameObjectList.size();
					RenderObjAmountData->value = m_RenderGameObjectList.size();*/
			obj->Update();

		}
	}
	DebugTextOnOff();
}

void TestScene::Render()
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

void TestScene::LoadObject(vector<MeshBuildInfo*> MeshInfoList, shared_ptr<ObjectDataTable> ObjData)
{
	// JSON으로 읽어와서 추가되어야하는 오브젝트들. ASE로 읽어온 데이터와 연동시킨다.
	for (auto& _MeshInfo : MeshInfoList)
	{
		shared_ptr<GameObject> _NewGameObject = make_shared<GameObject>(_MeshInfo->IDName);
		_NewGameObject->AddComponent<Transform>();
		_NewGameObject->AddComponent<MeshRenderer>();
		_NewGameObject->AddComponent<BoxCollider>();

		if (_MeshInfo->IDName == L"Smith")
		{
			_NewGameObject->AddComponent<Movement>();
			_NewGameObject->GetComponent<Movement>()->SetMoveSpeed(0.04f);
			_NewGameObject->GetComponent<Movement>()->SetRotateSpeed(10.0f);
			_NewGameObject->AddComponent<AnimState>();
			//_NewGameObject->GetComponent<Transform>()->LookAt(Vector3(0, 0, 0), Vector3(0, 0, -10.f), Vector3(0, 1.0f, 0));
		}

		for (size_t i = 0; i < ObjData->m_DataCount; i++)
		{
			//_NewGameObject->GetComponent<Transform>()->SetLocalPosition({ ObjData->m_Position[i].x, ObjData->m_Position[i].y, ObjData->m_Position[i].z });
			if (_MeshInfo->IDName == ObjData->m_Name[i])
			{
				/*_NewGameObject->GetComponent<Transform>()->SetLocalPosition({ _MeshInfo->Pos.x, _MeshInfo->Pos.y , _MeshInfo->Pos.z });
				_NewGameObject->GetComponent<Transform>()->SetLocalRotate({ _MeshInfo->Rot.x, _MeshInfo->Rot.y , _MeshInfo->Rot.z });
				_NewGameObject->GetComponent<Transform>()->SetLocalScale({ _MeshInfo->Scale.x, _MeshInfo->Scale.y , _MeshInfo->Scale.z });*/
				//_MeshInfo->Pos = ObjData->m_Position[i];
				_MeshInfo->isDraw = true;
				_MeshInfo->ObjType = ObjData->m_Type[i];
			}

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
		
		if (_NewGameObject->GetName() == L"Speech_bubble")
		{
			_NewGameObject->GetComponent<Transform>()->SetLocalRotate({ PI / 4, -PI / 4 , 0 });
			_NewGameObject->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = false;
		}
		else if (_NewGameObject->GetName() == L"Case_bubble" ||
			_NewGameObject->GetName() == L"Fire_bubble" ||
			_NewGameObject->GetName() == L"Frame_bubble" ||
			_NewGameObject->GetName() == L"Anvil_bubble" ||
			_NewGameObject->GetName() == L"Enchant_bubble" ||
			_NewGameObject->GetName() == L"Counter_bubble" ||
			_NewGameObject->GetName() == L"Dot_bubble" ||
			_NewGameObject->GetName() == L"Space_bar")
		{
			_NewGameObject->GetComponent<Transform>()->SetLocalRotate({ PI / 4, -PI / 4 , 0 });
			_NewGameObject->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = false;
		}
		else if (_NewGameObject->GetName() == L"Request_bubble")
		{
			_NewGameObject->GetComponent<Transform>()->SetLocalRotate({ PI / 4, -PI / 4 , 0 });
			_NewGameObject->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = false;
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
		else if (_NewGameObject->GetName() == L"Warrior_A" ||
			_NewGameObject->GetName() == L"Warrior_B" ||
			_NewGameObject->GetName() == L"Warrior_C" ||
			_NewGameObject->GetName() == L"Warrior_D")
		{
			_NewGameObject->GetComponent<Transform>()->SetLocalRotate({ 0, PI / 2 , 0 });
			_NewGameObject->GetComponent<MeshRenderer>()->GetMeshInfo()->isDraw = false;
			_NewGameObject->GetComponent<Transform>()->SetLocalPosition({ 9.5f, 0.f, -2.5f });
			m_Warrior->GetComponent<AIController>()->m_WarriorList.emplace_back(_NewGameObject);
		}
		else if (_NewGameObject->GetName() == L"Wall")
		{

		}

		m_GameObjectList.emplace_back(_NewGameObject);
	}
}

void TestScene::CreateUI()
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

	for (auto& _UI : UIManager::GetInstance()->m_ReviewUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}
	for (auto& _UI : UIManager::GetInstance()->m_CountDownUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}

	for (auto& _UI : UIManager::GetInstance()->m_EndingUI)
	{
		shared_ptr<GameObject> _NewUI = make_shared<GameObject>(_UI->Name);
		_NewUI->AddComponent<Transform>();
		_NewUI->AddComponent<UIRenderer>();
		_NewUI->GetComponent<UIRenderer>()->SetUIData(_UI->Name, _UI->UIType, _UI->FileName, _UI->Data, _UI->Type, _UI->isDraw, _UI->isMovable);


		m_GameObjectList.emplace_back(_NewUI);
	}
	/*for (auto& _ButtonUI : UIManager::GetInstance()->m_ButtonUI)
	{
		shared_ptr<GameObject> _NewButtonUI = make_shared<GameObject>(_ButtonUI->UIName);
		_NewButtonUI->AddComponent<Transform>();
		_NewButtonUI->AddComponent<UIRenderer>();
		_NewButtonUI->GetComponent<UIRenderer>()->SetUIData(_ButtonUI->UIName, _ButtonUI->UIType, _ButtonUI->FileName, _ButtonUI->left, _ButtonUI->top, _ButtonUI->right, _ButtonUI->bottom, _ButtonUI->isDraw);

		m_GameObjectList.emplace_back(_NewButtonUI);
	}

	for (auto& _RequestUI : UIManager::GetInstance()->m_RequestUI)
	{
		shared_ptr<GameObject> _NewRequestUI = make_shared<GameObject>(_RequestUI->UIName);
		_NewRequestUI->AddComponent<Transform>();
		_NewRequestUI->AddComponent<UIRenderer>();
		_NewRequestUI->GetComponent<UIRenderer>()->SetUIData(_RequestUI->UIName, _RequestUI->UIType, _RequestUI->FileName, _RequestUI->left, _RequestUI->top, _RequestUI->right, _RequestUI->bottom, _RequestUI->isDraw);

		m_GameObjectList.emplace_back(_NewRequestUI);
	}

	for (auto& _BookUI : UIManager::GetInstance()->m_Book)
	{
		shared_ptr<GameObject> _NewBookUI = make_shared<GameObject>(_BookUI->UIName);
		_NewBookUI->AddComponent<Transform>();
		_NewBookUI->AddComponent<UIRenderer>();
		_NewBookUI->GetComponent<UIRenderer>()->SetUIData(_BookUI->UIName, _BookUI->UIType, _BookUI->FileName, _BookUI->left, _BookUI->top, _BookUI->right, _BookUI->bottom, _BookUI->isDraw);

		m_GameObjectList.emplace_back(_NewBookUI);
	}


	for (auto& _OreUI : UIManager::GetInstance()->m_OreUI)
	{
		shared_ptr<GameObject> _NewOreUI = make_shared<GameObject>(_OreUI->UIName);
		_NewOreUI->AddComponent<Transform>();
		_NewOreUI->AddComponent<UIRenderer>();
		_NewOreUI->GetComponent<UIRenderer>()->SetUIData(_OreUI->UIName, _OreUI->UIType, _OreUI->FileName, _OreUI->left, _OreUI->top, _OreUI->right, _OreUI->bottom, _OreUI->isDraw);

		m_GameObjectList.emplace_back(_NewOreUI);
	}

	for (auto& _MeltingUI : UIManager::GetInstance()->m_MeltingUI)
	{
		shared_ptr<GameObject> _NewMeltingUI = make_shared<GameObject>(_MeltingUI->UIName);
		_NewMeltingUI->AddComponent<Transform>();
		_NewMeltingUI->AddComponent<UIRenderer>();
		_NewMeltingUI->GetComponent<UIRenderer>()->SetUIData(_MeltingUI->UIName, _MeltingUI->UIType, _MeltingUI->FileName, _MeltingUI->left, _MeltingUI->top, _MeltingUI->right, _MeltingUI->bottom, _MeltingUI->isDraw);

		m_GameObjectList.emplace_back(_NewMeltingUI);
	}

	for (auto& _PouringUI : UIManager::GetInstance()->m_PouringUI)
	{
		shared_ptr<GameObject> _NewPouringUI = make_shared<GameObject>(_PouringUI->UIName);
		_NewPouringUI->AddComponent<Transform>();
		_NewPouringUI->AddComponent<UIRenderer>();
		_NewPouringUI->GetComponent<UIRenderer>()->SetUIData(_PouringUI->UIName, _PouringUI->UIType, _PouringUI->FileName, _PouringUI->left, _PouringUI->top, _PouringUI->right, _PouringUI->bottom, _PouringUI->isDraw);

		m_GameObjectList.emplace_back(_NewPouringUI);
	}

	for (auto& _HammeringUI : UIManager::GetInstance()->m_HammeringUI)
	{
		shared_ptr<GameObject> _NewHammeringUI = make_shared<GameObject>(_HammeringUI->UIName);
		_NewHammeringUI->AddComponent<Transform>();
		_NewHammeringUI->AddComponent<UIRenderer>();
		_NewHammeringUI->GetComponent<UIRenderer>()->SetUIData(_HammeringUI->UIName, _HammeringUI->UIType, _HammeringUI->FileName, _HammeringUI->left, _HammeringUI->top, _HammeringUI->right, _HammeringUI->bottom, _HammeringUI->isDraw);

		m_GameObjectList.emplace_back(_NewHammeringUI);
	}

	for (auto& _InchantSelUI : UIManager::GetInstance()->m_InchantSelUI)
	{
		shared_ptr<GameObject> _NewInchantSelUI = make_shared<GameObject>(_InchantSelUI->UIName);
		_NewInchantSelUI->AddComponent<Transform>();
		_NewInchantSelUI->AddComponent<UIRenderer>();
		_NewInchantSelUI->GetComponent<UIRenderer>()->SetUIData(_InchantSelUI->UIName, _InchantSelUI->UIType, _InchantSelUI->FileName, _InchantSelUI->left, _InchantSelUI->top, _InchantSelUI->right, _InchantSelUI->bottom, _InchantSelUI->isDraw);

		m_GameObjectList.emplace_back(_NewInchantSelUI);
	}

	for (auto& _InchantHitUI : UIManager::GetInstance()->m_InchantHitUI)
	{
		shared_ptr<GameObject> _NewInchantHitUI = make_shared<GameObject>(_InchantHitUI->UIName);
		_NewInchantHitUI->AddComponent<Transform>();
		_NewInchantHitUI->AddComponent<UIRenderer>();
		_NewInchantHitUI->GetComponent<UIRenderer>()->SetUIData(_InchantHitUI->UIName, _InchantHitUI->UIType, _InchantHitUI->FileName, _InchantHitUI->left, _InchantHitUI->top, _InchantHitUI->right, _InchantHitUI->bottom, _InchantHitUI->isDraw);

		m_GameObjectList.emplace_back(_NewInchantHitUI);
	}

	for (auto& _HandGuardUI : UIManager::GetInstance()->m_HandGuardUI)
	{
		shared_ptr<GameObject> _NewHandGuardUI = make_shared<GameObject>(_HandGuardUI->UIName);
		_NewHandGuardUI->AddComponent<Transform>();
		_NewHandGuardUI->AddComponent<UIRenderer>();
		_NewHandGuardUI->GetComponent<UIRenderer>()->SetUIData(_HandGuardUI->UIName, _HandGuardUI->UIType, _HandGuardUI->FileName, _HandGuardUI->left, _HandGuardUI->top, _HandGuardUI->right, _HandGuardUI->bottom, _HandGuardUI->isDraw);

		m_GameObjectList.emplace_back(_NewHandGuardUI);
	}*/
}




void TestScene::DebugTextOnOff()
{
	if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F10) == KeyState::Down)
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

void TestScene::BgmChange()
{
	if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F1) == KeyState::Down)
	{
		SoundManager::GetInstance()->BgmChange(0);
	}
	else if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F2) == KeyState::Down)
	{
		SoundManager::GetInstance()->BgmChange(1);
	}
	else if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F3) == KeyState::Down)
	{
		SoundManager::GetInstance()->BgmChange(2);
	}
	else if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F4) == KeyState::Down)
	{
		SoundManager::GetInstance()->BgmChange(3);
	}
	else if (InputManager::GetInstance()->GetCurrentKeyState(VirtualKey::F5) == KeyState::Down)
	{
		SoundManager::GetInstance()->BgmChange(4);
	}
}

/*
TestScene 생성자안에 있던거
	//m_CameraInfo = make_shared<CameraInfo>();

	//// 메인 카메라
	//shared_ptr<GameObject> _CameraObj = make_shared<GameObject>(L"MainCamera");
	//_CameraObj->AddComponent<Transform>();
	//_CameraObj->AddComponent<Camera>();
	//_CameraObj->GetComponent<Camera>()->SetPlayer(nullptr);

	//_CameraObj->GetComponent<Camera>()->SetCameraInfo(m_CameraInfo);

	//m_CameraInfo->Pos = { 20, 20, -20 };
	//m_CameraInfo->Up = { 0, 1.0, 0 };
	//m_CameraInfo->Target = { 0, 0, 0 };

	//_CameraObj->GetComponent<Transform>()->LookAt(m_CameraInfo->Pos, m_CameraInfo->Target, m_CameraInfo->Up);

	//m_GameObjectList.emplace_back(_CameraObj);



	//UI TEST
	/*shared_ptr<GameObject> _UI01 = make_shared<GameObject>(L"UI01");
	_UI01->AddComponent<Transform>();
	_UI01->AddComponent<UI>();
	_UI01->AddComponent<UIMovement>();
	_UI01->AddComponent<UIHitFunc>();
	_UI01->AddComponent<Quest>();
	_UI01->AddComponent<MeshRenderer>();

	m_GameObjectList.emplace_back(_UI01);

	_UserObj->GetComponent<Controller>()->SetTargetUI(_UI01);*/	// 작용할 UI 설정

	// Genji
	/*shared_ptr<GameObject> _genjiObj = make_shared<GameObject>(L"genji");
	m_GameObjectList.emplace_back(_genjiObj);

	_genjiObj->AddComponent<Transform>();
	_genjiObj->AddComponent<MeshRenderer>();

	_genjiObj->GetComponent<MeshRenderer>()->SetName(L"genji");

	_genjiObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Name = "genji";
	_genjiObj->GetComponent<MeshRenderer>()->GetMeshInfo()->ObjType = eObjType::Animation;
	_genjiObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Pos =
		_genjiObj->GetComponent<Transform>()->GetLocalPosition();
	_genjiObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Rot =
		_genjiObj->GetComponent<Transform>()->GetLocalRotate();
	_genjiObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Scale =
		_genjiObj->GetComponent<Transform>()->GetLocalScale();*/

		// Joe
		//shared_ptr<GameObject> _joeObj = make_shared<GameObject>(L"joe");
		//m_GameObjectList.emplace_back(_joeObj);

		//_joeObj->AddComponent<Transform>();
		//_joeObj->AddComponent<MeshRenderer>();

		//_joeObj->GetComponent<MeshRenderer>()->SetName(L"joe");

		//_joeObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Name = "joe";
		//_joeObj->GetComponent<MeshRenderer>()->GetMeshInfo()->ObjType = eObjType::Animation;
		//_joeObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Pos =
		//	_joeObj->GetComponent<Transform>()->GetLocalPosition();
		//_joeObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Rot =
		//	_joeObj->GetComponent<Transform>()->GetLocalRotate();
		//_joeObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Scale =
		//	_joeObj->GetComponent<Transform>()->GetLocalScale();

		//// Pig


		/// 치우랑 재영이가 해야 할 일들이고 월요일날 좀 보자...

		/*
		/// 1. 아무것도 없는 것에서 새로 만들어야 할 때:

		/// 돼지 GameObject를 만든다. 그런데 당연히 돼지는 1개의 GameObject(Entity)만으로 만들 수가 없다.
		shared_ptr<GameObject> _pigObj = make_shared<GameObject>(L"pig");	// 그래서 이게 돼지 FBX(ASE)의 root로 간주한다.
		m_GameObjectList.emplace_back(_pigObj);

		_pigObj->AddComponent<Transform>();
		MeshFilter* _pigSkinMesh = _pigObj->AddComponent<MeshFilter>();
		_pigSkinMesh->UUIndex = HimeEngine::FindUUIDFromName("PigSkin");

		MeshRenderer* _pigSkinRenderer = _pigObj->AddComponent<MeshRenderer>();
		_pigSkinRenderer->SetTexture(...);
		_pigSkinRenderer->SetShaderOption(32.0f);


		/// 2. FBX나 ASE를 로드해서 자동으로 계층도에 만들어지는 것:

		/// 히메엔진에 요청해서, 렌더링 큐에 있는 DXObjectBase의 관계(예: NodeIno)를 얻어온다.
		/// 그것을 기준으로 Hierarchy를 만들어준다.
		/// GameObject들의 관계를 만든다.
		/// 각 GameObject에 MeshFilter를 붙인다. 그리고 히메엔진의 DXObject들의 UUID를 얻어와서 숫자만 가지고 있는다.


		/// 나중에 그릴 때에는 거꾸로 모둔 GameObject들의 컴포넌트(MeshFilter/Meshrenderer)를 참조해서 그린다.
		*/

		//shared_ptr<GameObject> _pigObj = make_shared<GameObject>(L"pig");	// 그래서 이게 돼지 FBX(ASE)의 root로 간주한다.
		//m_GameObjectList.emplace_back(_pigObj);

		//_pigObj->AddComponent<Transform>();
		//_pigObj->AddComponent<MeshRenderer>();
	 //	_pigObj->GetComponent<MeshRenderer>()->SetName(L"pig");
	 //
	 //	_pigObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Name = "pig";
	 //	_pigObj->GetComponent<MeshRenderer>()->GetMeshInfo()->ObjType = eObjType::Skinning;
	 //	_pigObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Pos =
	 //		_pigObj->GetComponent<Transform>()->GetLocalPosition();
	 //	_pigObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Rot =
	 //		_pigObj->GetComponent<Transform>()->GetLocalRotate();
	 //	_pigObj->GetComponent<MeshRenderer>()->GetMeshInfo()->Scale =
	 //		_pigObj->GetComponent<Transform>()->GetLocalScale();

		/*shared_ptr<GameObject> _Axis = make_shared<GameObject>(L"Axis");
		_Axis->AddComponent<Transform>();
		_Axis->AddComponent<MeshRenderer>();
		_Axis->GetComponent<MeshRenderer>()->SetName(L"Axis");
		_Axis->GetComponent<MeshRenderer>()->GetMeshInfo()->Name = "Axis";
		_Axis->GetComponent<MeshRenderer>()->GetMeshInfo()->ObjType = eObjType::Helper;
		_Axis->GetComponent<MeshRenderer>()->GetMeshInfo()->Pos = _Axis->GetComponent<Transform>()->GetLocalPosition();
		_Axis->GetComponent<MeshRenderer>()->GetMeshInfo()->Rot = _Axis->GetComponent<Transform>()->GetLocalRotate();
		_Axis->GetComponent<MeshRenderer>()->GetMeshInfo()->Scale = _Axis->GetComponent<Transform>()->GetLocalScale();

		m_GameObjectList.emplace_back(_Axis);


		shared_ptr<GameObject> _Grid = make_shared<GameObject>(L"Grid");
		_Grid->AddComponent<Transform>();
		_Grid->AddComponent<MeshRenderer>();
		_Grid->GetComponent<MeshRenderer>()->SetName(L"Grid");
		_Grid->GetComponent<MeshRenderer>()->GetMeshInfo()->Name = "Grid";
		_Grid->GetComponent<MeshRenderer>()->GetMeshInfo()->ObjType = eObjType::Helper;
		_Grid->GetComponent<MeshRenderer>()->GetMeshInfo()->Pos = _Grid->GetComponent<Transform>()->GetLocalPosition();
		_Grid->GetComponent<MeshRenderer>()->GetMeshInfo()->Rot = _Grid->GetComponent<Transform>()->GetLocalRotate();
		_Grid->GetComponent<MeshRenderer>()->GetMeshInfo()->Scale = _Grid->GetComponent<Transform>()->GetLocalScale();

		m_GameObjectList.emplace_back(_Grid);*/





