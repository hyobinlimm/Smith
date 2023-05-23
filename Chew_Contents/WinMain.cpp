#include "CCPch.h"
#include "ClientSystem.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow){
	// Client 객체 생성
	ClientSystem* Client = new ClientSystem;
	if (!Client)
	{
		return -1;
	}

	// Client 객체 초기화 및 실행
	if (Client->Initialize())
	{
		Client->Run();
	}

	// Client 객체 종료 및 메모리 반환
	Client->Shutdown();
	delete Client;
	Client = nullptr;

	return 0;
}