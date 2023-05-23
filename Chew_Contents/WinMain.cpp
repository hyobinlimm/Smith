#include "CCPch.h"
#include "ClientSystem.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow){
	// Client ��ü ����
	ClientSystem* Client = new ClientSystem;
	if (!Client)
	{
		return -1;
	}

	// Client ��ü �ʱ�ȭ �� ����
	if (Client->Initialize())
	{
		Client->Run();
	}

	// Client ��ü ���� �� �޸� ��ȯ
	Client->Shutdown();
	delete Client;
	Client = nullptr;

	return 0;
}