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

	// ������Ʈ ������ ������ JSON������ �о�´�.
	shared_ptr<ObjectDataTable> m_ObjData;

	// �о�� �����͵��� �־ �׷��Ƚ� �������� �ѱ� �κе�.
	vector<FilePath*> m_FilePath;
	vector<MeshBuildInfo*> m_BuildInfo;
	
	TextData* m_FpsTest;
};

