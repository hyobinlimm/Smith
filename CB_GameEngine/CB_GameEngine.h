#pragma once

#include "HimeRendererStructs.h"

class HimeEngine;
class ObjectDataTable;

class CB_GameEngine
{
public:
	CB_GameEngine();
	~CB_GameEngine();
public:
	void Init(HWND hWnd, int width, int height);
	void Update();
	void Render();

	void SetScreen(WPARAM wparam, LPARAM lparam, bool resizing, bool sizeMove);

private:
	HimeEngine* m_HimeEngine;

	// 오브젝트 데이터 테이플 JSON파일을 읽어온다.
	shared_ptr<ObjectDataTable> m_ObjData;

	// 읽어온 데이터들을 넣어서 그래픽스 엔진으로 넘길 부분들.
	vector<FilePath*> m_FilePath;
	vector<MeshBuildInfo*> m_BuildInfo;
	
	TextData* m_FpsTest;
};

