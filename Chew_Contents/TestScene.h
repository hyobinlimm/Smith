#pragma once

#include "Scene.h"

/// <summary>
/// CB_GameEngine�� Scene�� ��ӹ޾� ������ �׽�Ʈ ��.
/// �̰����� ���� �� ������Ʈ���� �������ش�.
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

	//���� ���� ����
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);

	shared_ptr<GameObject> m_Warrior;
};

