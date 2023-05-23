#pragma once

class CB_GameEngine;

class ClientSystem
{
public:
	ClientSystem();
	ClientSystem(const ClientSystem&);
	~ClientSystem();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(const int&, const int&);
	void ShutdownWindows();
	void RecalcWindowSize();

private:
	LPCWSTR m_applicationName = nullptr;
	HINSTANCE m_hinstance = nullptr;
	HWND m_hwnd = nullptr;

	const bool FULL_SCREEN = false;

	const int m_ScreenWidth = 2560;
	const int m_ScreenHeight = 1440;

	static bool m_Resizing;
	static bool m_SizeChange;
	shared_ptr<CB_GameEngine> m_GameEngine;

	static shared_ptr<CB_GameEngine> m_GameEngineInstance;

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};

static ClientSystem* ApplicationHandle = 0;