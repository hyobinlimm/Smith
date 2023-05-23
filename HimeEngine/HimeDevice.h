#pragma once

#include "DirectXDefine.h"

///
/// DirectX 객체를 초기화 하는 클래스
/// 딱 한번만 실행 됨.
/// 초기화 된 DirectX 객체들을 들고 있음.
/// 

class HimeDevice
{
private:
	HimeDevice();
	~HimeDevice();

#pragma region MK1
	// view들만 모아 놓을 함수에 옮겨 놓자. 
private:
	/// Render State
	// 미리 여러 세트를 만들어두고 스위칭한다.
	ID3D11RasterizerState* m_pWireframeRS;
	ID3D11RasterizerState* m_pSolidRS;

	// 폰트때문에 뎁스스탠실 스테이트가 강제가 됐다.
	ID3D11DepthStencilState* m_pNormalDSS;
#pragma endregion

private:
	// 초기화 함수들
	HRESULT CreateDeviceAndSwapChain(HWND hWnd, int screenWidth, int screenHeight);
	HRESULT CreateRenderTargetView(int screenWidth, int screenHeight);

public: 
	// getset 함수들
	ID3D11Device* GetD3dDevice() const { return m_pD3dDevice; }
	ID3D11DeviceContext* GetDeviceContext() const { return m_pDeviceContext; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
	void SetDepthStencilView(ID3D11DepthStencilView* val) { m_pDepthStencilView = val; }


#pragma region MK1
	// view들만 모아 놓을 함수에 옮겨 놓자. 
	ID3D11RasterizerState* GetWireframeRS() const { return m_pWireframeRS; }
	ID3D11RasterizerState* GetSolidRS() const { return m_pSolidRS; }
	ID3D11DepthStencilState* GetNormalDSS() const { return m_pNormalDSS; }
#pragma endregion
public:
	static HimeDevice* GetInst();
	static void DestroyInst();

	void Initialize(HWND hWnd, int screenWidth, int screenHeight);
	void OnResize(int screenWidth, int screenHeight);
	void CreateRenderState();
	
	// Render에 필요한 함수
	void DXClearRenderTargetView();
	void DXPresent();


private:
	/// singleton
	static HimeDevice* m_pGPInst;

	/// 참조 디바이스 및 스왑체인에 필요한 변수 
	IDXGISwapChain* m_pSwapChain;
	ID3D11Device* m_pD3dDevice;									// - 생성
	ID3D11DeviceContext* m_pDeviceContext;						// - 사용
	ID3D11RenderTargetView* m_pRenderTarget;

	UINT m_4xMsaaQuality; //msaa 확인용
	bool m_bEnable4xMsaa;

	/// 텍스처에 필요한 변수
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepthStencilBuffer;

	/// 뷰포트
	D3D11_VIEWPORT m_ScreenViewport;

	int m_ScreenWidth;
	int m_ScreenHeight;
};

