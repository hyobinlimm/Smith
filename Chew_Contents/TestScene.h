#pragma once

#include "Scene.h"

/// <summary>
/// CB_GameEngine의 Scene을 상속받아 생성될 테스트 씬.
/// 이곳에서 씬에 들어갈 오브젝트들을 관리해준다.
/// </summary>

class HimeEngine;
class DebugingData;
enum class GameMode;

class TestScene : public Scene
{
public:
	TestScene(wstring sceneName);
	virtual ~TestScene() {};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void LoadObject(vector<MeshBuildInfo*> MeshInfoList, shared_ptr<ObjectDataTable> ObjData) override;

private:
	virtual void CreateUI() override;

	void DebugTextOnOff();

	void BgmChange();



private:
	shared_ptr<TextData> CharacterPosData;
	shared_ptr<TextData> CharacterRotData;
	shared_ptr<TextData> ObjAmountData;
	shared_ptr<TextData> RenderObjAmountData;

	//라디안 계산용 변수
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);

	shared_ptr<GameObject> m_Warrior;
};

