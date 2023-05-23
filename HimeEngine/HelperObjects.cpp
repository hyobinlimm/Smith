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
	// �Է� ��ġ ��ü ����
	m_pD3dImmediateContext->IASetInputLayout(InputLayouts::PosColor);
	m_pD3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// �ε������ۿ� ���ؽ����� ����
	UINT stride = sizeof(VertexStruct::PosColor);
	UINT offset = 0;
	m_pD3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pD3dImmediateContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	/// WVP TM���� ����
	// Set constants
	XMMATRIX worldViewProj = m_World * m_View * m_Proj;
	Effects::PosColorFX->SetWorldViewProj(worldViewProj);

	// ����������Ʈ
	m_pD3dImmediateContext->RSSetState(m_pRenderstate);

	// ��ũ����...
	ID3DX11EffectTechnique* _Tech = Effects::PosColorFX->Tech;
	
	D3DX11_TECHNIQUE_DESC techDesc;
	_Tech->GetDesc(&techDesc);

	// �����н���...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		_Tech->GetPassByIndex(p)->Apply(0, m_pD3dImmediateContext);

		// ��ü�� �ε��� ����� ���缭 context�� �־�
		m_pD3dImmediateContext->DrawIndexed(m_IndexSize, 0, 0);
	}
}

void HelperObjects::SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB)
{
	m_pVB = VB;
	m_pIB = IB;
}
