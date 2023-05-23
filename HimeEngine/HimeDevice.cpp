#include "HimeDevice.h"
#include <DirectXColors.h>
#include <wrl.h>

HimeDevice* HimeDevice::m_pGPInst = nullptr;

HimeDevice::HimeDevice()
	: m_pSwapChain(nullptr), m_pD3dDevice(nullptr), m_pDeviceContext(nullptr), m_pRenderTarget(nullptr),
	m_bEnable4xMsaa(false), m_pDepthStencilView(nullptr), m_pDepthStencilBuffer(nullptr),
	m_pWireframeRS(nullptr), m_pSolidRS(nullptr), m_pNormalDSS(nullptr)
{
	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
}

HimeDevice::~HimeDevice()
{
	ReleaseCOM(m_pWireframeRS);
	ReleaseCOM(m_pSolidRS);
	ReleaseCOM(m_pNormalDSS);

	ReleaseCOM(m_pRenderTarget);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pSwapChain);
	ReleaseCOM(m_pDepthStencilBuffer);

	// Restore all default settings.
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
	}

	ReleaseCOM(m_pDeviceContext);
	ReleaseCOM(m_pD3dDevice);
}

HRESULT HimeDevice::CreateDeviceAndSwapChain(HWND hWnd, int screenWidth, int screenHeight)
{
	// 1-1. ���� ü�ο� ���� �ʱ� �Ű� ������ ����
	DXGI_SWAP_CHAIN_DESC _sd;
	ZeroMemory(&_sd, sizeof(_sd));
	_sd.BufferCount = 1;
	_sd.BufferDesc.Width = screenWidth;
	_sd.BufferDesc.Height = screenHeight;
	_sd.BufferDesc.RefreshRate.Numerator = 60;
	_sd.BufferDesc.RefreshRate.Denominator = 1;
	_sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (m_bEnable4xMsaa)
	{
		_sd.SampleDesc.Count = 4;
		_sd.SampleDesc.Quality = m_bEnable4xMsaa - 1;
	}
	else
	{
		_sd.SampleDesc.Count = 1;
		_sd.SampleDesc.Quality = 0;

	}

	_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_sd.OutputWindow = hWnd;
	_sd.Windowed = TRUE;
	_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	_sd.Flags = 0;

	// 1-2. ���ø����̼ǿ� �ʿ��� ����� �����ϴ� ��� ������ ��û.
	//		Direct3D 11 ��Ÿ�ӿ� ���� ���� ����̽��� ���� �� �ִ�. 
	//		D3D_FEATURE_LEVEL_11_0 �̻��̸� IDXGIFactory1�� ����ؾ� �Ѵ�. 
	const D3D_FEATURE_LEVEL _lvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
									  D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
									  D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };

	UINT _createDeviceFlags = 0;

#ifdef _DEBUG
	_createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//UINT numLevelsRequested = 1;
	D3D_FEATURE_LEVEL FeatureLevel;

	// 1-3.  D3D11CreateDeviceAndSwapChain�� ȣ���Ͽ� ����̽��� �����. 
	HRESULT	hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			&_lvl[1],
			_countof(_lvl) - 1,
			D3D11_SDK_VERSION,
			&_sd,
			&m_pSwapChain,
			&m_pD3dDevice,
			&FeatureLevel,
			&m_pDeviceContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"CreateDeviceAndSwapChain failed!", 0, 0);
		return S_FALSE;
	}
	
	return S_OK;
}

HRESULT HimeDevice::CreateRenderTargetView(int screenWidth, int screenHeight)
{
	// ID3D11Device::CreateRenderTargetView �� ȣ���Ͽ� ���� ��� ���⸦ �����,
	// ID3D11DeviceContext:: OMSetRenderTargets �� ȣ�� �Ͽ� �� ���۸� ���� ������� ���ε��մϴ�.

	// ComPtr�� ���� ��ü�� GetAddressOf() �Լ��� ����ؼ� ��ü�� �޾ƿ´�. 
	// �� ��ü�� ����Ҷ��� get()�� �ؾ��Ѵ�. 
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _back;
	m_pD3dDevice->CreateRenderTargetView(_back.Get(), NULL, &m_pRenderTarget);

	// Bind the view
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTarget, NULL);

	// ����Ʈ�� ����� ���� ����� � �κ��� ǥ������ �����մϴ�. 
	// D3D11_VIEWPORT ���� �� ����Ͽ� ����Ʈ�� �����ϰ� 
	// ID3D11DeviceContext::RSSetViewports �޼��带 ����Ͽ� ����Ʈ�� �����մϴ�.

	// Setup the viewport
	D3D11_VIEWPORT _vp;
	_vp.Width = static_cast<float>(screenWidth);
	_vp.Height = static_cast<float>(screenHeight);
	_vp.MinDepth = 0.0f;
	_vp.MaxDepth = 1.0f;
	_vp.TopLeftX = 0;
	_vp.TopLeftY = 0;
	m_pDeviceContext->RSSetViewports(1, &_vp);

	return S_OK;
}

void HimeDevice::CreateRenderState()
{
	// Render State �� Rasterizer State
	D3D11_RASTERIZER_DESC _solidDesc;
	ZeroMemory(&_solidDesc, sizeof(D3D11_RASTERIZER_DESC));
	_solidDesc.FillMode = D3D11_FILL_SOLID;
	_solidDesc.CullMode = D3D11_CULL_NONE;
	_solidDesc.FrontCounterClockwise = false;
	_solidDesc.DepthClipEnable = true;

	HR(m_pD3dDevice->CreateRasterizerState(&_solidDesc, &m_pSolidRS));

	D3D11_RASTERIZER_DESC _wireframeDesc;
	ZeroMemory(&_wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	_wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	_wireframeDesc.CullMode = D3D11_CULL_BACK;
	_wireframeDesc.FrontCounterClockwise = false;
	_wireframeDesc.DepthClipEnable = true;

	HR(m_pD3dDevice->CreateRasterizerState(&_wireframeDesc, &m_pWireframeRS));

	// ��Ʈ�� DSS
	D3D11_DEPTH_STENCIL_DESC _equalsDesc;
	ZeroMemory(&_equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	_equalsDesc.DepthEnable = true;
	_equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// ���̹��ۿ� ����� �Ѵ�
	_equalsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	HR(m_pD3dDevice->CreateDepthStencilState(&_equalsDesc, &m_pNormalDSS));
}

HimeDevice* HimeDevice::GetInst()
{
	if (!m_pGPInst)
	{
		m_pGPInst = new HimeDevice();
	}

	return m_pGPInst;
}

void HimeDevice::DestroyInst()
{
	if (!m_pGPInst)
	{
		return;
	}
	
	delete m_pGPInst;
	m_pGPInst = nullptr;
}

void HimeDevice::Initialize(HWND hWnd, int screenWidth, int screenHeight)
{
	/// ���� ����̽� ����� 
	CreateDeviceAndSwapChain(hWnd, screenWidth, screenHeight);
	CreateRenderTargetView(screenWidth, screenHeight);
}

void HimeDevice::OnResize(int screenWidth, int screenHeight)
{
	assert(m_pDeviceContext);
	assert(m_pD3dDevice);
	assert(m_pSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCOM(m_pRenderTarget);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pDepthStencilBuffer);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> _back;
	// ComPtr�� �Լ��� ����������
	// Get �Լ��� ���� �� �۹��� ���۷��� ī��Ʈ�� �÷����� ���� �ٽ� ������ ����.

	// Resize the swap chain and recreate the render target view.
	HR(m_pSwapChain->ResizeBuffers((UINT)1, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)_back.GetAddressOf());
	HR(m_pD3dDevice->CreateRenderTargetView(_back.Get(), 0, &m_pRenderTarget));
	
	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC _depthStencilDesc;

	if (screenWidth == 0 && screenWidth == 0)
	{
		screenWidth = 1; 
		screenWidth = 1;
	}

	_depthStencilDesc.Width = screenWidth;
	_depthStencilDesc.Height = screenWidth;
	_depthStencilDesc.MipLevels = 1;
	_depthStencilDesc.ArraySize = 1;
	_depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (m_bEnable4xMsaa)
	{
		_depthStencilDesc.SampleDesc.Count = 4;
		_depthStencilDesc.SampleDesc.Quality = m_bEnable4xMsaa - 1;
	}
	// No MSAA
	else
	{
		_depthStencilDesc.SampleDesc.Count = 1;
		_depthStencilDesc.SampleDesc.Quality = 0;
	}

	_depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	_depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	_depthStencilDesc.CPUAccessFlags = 0;
	_depthStencilDesc.MiscFlags = 0;

	HR(m_pD3dDevice->CreateTexture2D(&_depthStencilDesc, 0, &m_pDepthStencilBuffer));
	
	if (m_pDepthStencilBuffer!=nullptr)
	{
		HR(m_pD3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView));
	}

	// Bind the render target view and depth/stencil view to the pipeline.
	/// ����Ÿ�ٺ�, ����/���ĽǺ並 ���������ο� ���ε��Ѵ�.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTarget, m_pDepthStencilView);

	// Set the viewport transform.
	/// ����Ʈ ��ȯ�� �����Ѵ�.
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(screenWidth);
	m_ScreenViewport.Height = static_cast<float>(screenHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;

	m_pDeviceContext->RSSetViewports(1, &m_ScreenViewport);
}

void HimeDevice::DXClearRenderTargetView()
{
	// �������� ����� �����ؼ� �ٽ� �׷��ִ� �Լ�
	//m_pDeviceContext->ClearRenderTargetView(m_pRenderTarget, DirectX::Colors::LightCoral);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTarget, XMVECTORF32{ 0.098039216f, 0.08627451f, 0.07450980f });
	// �������Ľ� �並 Ŭ�����Ѵ�.
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void HimeDevice::DXPresent()
{
	// ȭ�� �����ؼ� �ٽ� �׸�. 
	m_pSwapChain->Present(1, 0);
}
