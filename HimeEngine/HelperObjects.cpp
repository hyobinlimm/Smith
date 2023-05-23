#include "HelperObjects.h"
#include "ResourceManager.h"
#include "Effects.h"
#include "InputLayouts.h"
#include "HimeCamera.h"

HelperObjects::HelperObjects(ID3D11RasterizerState* pRS)
	: m_pRenderstate(pRS),
	m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj()
{
	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pD3dImmediateContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();
}

HelperObjects::~HelperObjects()
{
	ReleaseCOM(m_pRenderstate);

	ReleaseCOM(m_pVB);
	ReleaseCOM(m_pIB);
}

void HelperObjects::Initialize()
{
	
}

void HelperObjects::Update(HimeCamera* pCamera)
{
	XMMATRIX _identify = XMMatrixIdentity();
	XMMATRIX _view = pCamera->View();
	XMMATRIX _proj = pCamera->Proj();

	XMStoreFloat4x4(&m_World, _identify);
	XMStoreFloat4x4(&m_View, _view);
	XMStoreFloat4x4(&m_Proj, _proj);
}

void HelperObjects::Render()
{
	// 입력 배치 객체 셋팅
	m_pD3dImmediateContext->IASetInputLayout(InputLayouts::PosColor);
	m_pD3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT stride = sizeof(VertexStruct::PosColor);
	UINT offset = 0;
	m_pD3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pD3dImmediateContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	/// WVP TM등을 셋팅
	// Set constants
	XMMATRIX worldViewProj = m_World * m_View * m_Proj;
	Effects::PosColorFX->SetWorldViewProj(worldViewProj);

	// 렌더스테이트
	m_pD3dImmediateContext->RSSetState(m_pRenderstate);

	// 테크닉은...
	ID3DX11EffectTechnique* _Tech = Effects::PosColorFX->Tech;
	
	D3DX11_TECHNIQUE_DESC techDesc;
	_Tech->GetDesc(&techDesc);

	// 랜더패스는...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		_Tech->GetPassByIndex(p)->Apply(0, m_pD3dImmediateContext);

		// 객체의 인덱스 사이즈에 맞춰서 context에 넣어
		m_pD3dImmediateContext->DrawIndexed(m_IndexSize, 0, 0);
	}
}

void HelperObjects::SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB)
{
	m_pVB = VB;
	m_pIB = IB;
}
