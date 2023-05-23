#include "ObjectFactory.h"
#include "Effects.h"
#include "InputLayouts.h"
#include "HimeDevice.h"
#include "ResourceManager.h"
#include "GeometryBuffer.h"

ObjectFactory::ObjectFactory(ID3D11RasterizerState* pRS)
	: m_pRenderstate(pRS),
	m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_World(), m_View(), m_Proj()
{
	/// ���ڷ� �޾Ƽ� �������. ���� ����� ���� �־�� �ϳ�? 
	m_D3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_D3dImmediateContext = HimeDevice::GetInst()->GetDeviceContext();
	
	m_pResourceManager = ResourceManager::GetInst();
}

ObjectFactory::~ObjectFactory()
{
	ReleaseCOM(m_pVB);
	ReleaseCOM(m_pIB);
}

void ObjectFactory::Initialize(std::string name)
{
	BuildGeometryBuffers(name);
}

void ObjectFactory::Update(const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& proj)
{
	m_World = world;
	m_View = view;
	m_Proj = proj;
}

void ObjectFactory::Render()
{
	/// ���ڸ� �޾ƿͼ� �����غ���. 
	// �Է� ��ġ ��ü ����
	m_D3dImmediateContext->IASetInputLayout(InputLayouts::PosColor);
	m_D3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// �ε������ۿ� ���ؽ����� ����
	UINT stride = sizeof(VertexStruct::PosColor);
	UINT offset = 0;
	m_D3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_D3dImmediateContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	/// WVP TM���� ����
	// Set constants
	XMMATRIX worldViewProj = m_World * m_View * m_Proj;
	Effects::PosColorFX->SetWorldViewProj(worldViewProj);

	// ����������Ʈ
	m_D3dImmediateContext->RSSetState(m_pRenderstate);

	// ��ũ����...
	ID3DX11EffectTechnique* mTech = Effects::PosColorFX->Tech;
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	// �����н���...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, m_D3dImmediateContext);

		// ��ü�� �ε��� ����� ���缭 context�� �־�
		m_D3dImmediateContext->DrawIndexed(m_IndexSize, 0, 0);
	}
}

void ObjectFactory::BuildGeometryBuffers(std::string name)
{
	//  ���ؽ� ���ۿ� �ε��� ���۴� �̹� ���ҽ��Ŵ����� ��� �ִ�. 
	GeometryBuffer* meshBuffers = m_pResourceManager->m_GBufferList.find(name)->second;
	m_IndexSize = meshBuffers->GetIndexSize();
	m_pVB = meshBuffers->GetVertexBuffer();
	m_pIB = meshBuffers->GetIndexBuffer();
}

