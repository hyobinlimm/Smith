#pragma once

#include "Scene.h"

class GameScene : public Scene
{
public:
	GameScene(wstring sceneName);
	virtual ~GameScene() {};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void LoadObject(vector<MeshBuildInfo*> MeshInfoList, shared_ptr<ObjectDataTable> ObjData) override;

private:
	virtual void CreateUI() override;

	void DebugTextOnOff();

private:
	shared_ptr<TextData> CharacterPosData;
	shared_ptr<TextData> CharacterRotData;
	//라디안 계산용 변수
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);
};

