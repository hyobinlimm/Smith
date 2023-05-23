#pragma once

#include "DirectXDefine.h"

///
/// DirectX ��ü�� �ʱ�ȭ �ϴ� Ŭ����
/// �� �ѹ��� ���� ��.
/// �ʱ�ȭ �� DirectX ��ü���� ��� ����.
/// 

class HimeDevice
{
private:
	HimeDevice();
	~HimeDevice();

#pragma region MK1
	// view�鸸 ��� ���� �Լ��� �Ű� ����. 
private:
	/// Render State
	// �̸� ���� ��Ʈ�� �����ΰ� ����Ī�Ѵ�.
	ID3D11RasterizerState* m_pWireframeRS;
	ID3D11RasterizerState* m_pSolidRS;

	// ��Ʈ������ �������Ľ� ������Ʈ�� ������ �ƴ�.
	ID3D11DepthStencilState* m_pNormalDSS;
#pragma endregion

private:
	// �ʱ�ȭ �Լ���
	HRESULT CreateDeviceAndSwapChain(HWND hWnd, int screenWidth, int screenHeight);
	HRESULT CreateRenderTargetView(int screenWidth, int screenHeight);

public: 
	// getset �Լ���
	ID3D11Device* GetD3dDevice() const { return m_pD3dDevice; }
	ID3D11DeviceContext* GetDeviceContext() const { return m_pDeviceContext; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
	void SetDepthStencilView(ID3D11DepthStencilView* val) { m_pDepthStencilView = val; }


#pragma region MK1
	// view�鸸 ��� ���� �Լ��� �Ű� ����. 
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
	
	// Render�� �ʿ��� �Լ�
	void DXClearRenderTargetView();
	void DXPresent();


private:
	/// singleton
	static HimeDevice* m_pGPInst;

	/// ���� ����̽� �� ����ü�ο� �ʿ��� ���� 
	IDXGISwapChain* m_pSwapChain;
	ID3D11Device* m_pD3dDevice;									// - ����
	ID3D11DeviceContext* m_pDeviceContext;						// - ���
	ID3D11RenderTargetView* m_pRenderTarget;

	UINT m_4xMsaaQuality; //msaa Ȯ�ο�
	bool m_bEnable4xMsaa;

	/// �ؽ�ó�� �ʿ��� ����
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepthStencilBuffer;

	/// ����Ʈ
	D3D11_VIEWPORT m_ScreenViewport;

	int m_ScreenWidth;
	int m_ScreenHeight;
};

