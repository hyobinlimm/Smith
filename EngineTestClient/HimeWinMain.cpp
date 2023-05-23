#include "framework.h"
#include "HimeWinMain.h"
#include "GameProcess.h"
#include <crtdbg.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	HINSTANCE hInstance/* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */)
{
	//_CrtSetBreakAlloc(159);
	GameProcess* pGameProcess = new GameProcess();
	pGameProcess->Initialize(hInstance);
	pGameProcess->Loop();
	pGameProcess->Finalize();
	_CrtDumpMemoryLeaks();
}