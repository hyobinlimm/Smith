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
	/// 인자로 받아서 사용하자. 굳이 멤버로 갖고 있어야 하나? 
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
	/// 인자를 받아와서 생각해보자. 
	// 입력 배치 객체 셋팅
	m_D3dImmediateContext->IASetInputLayout(InputLayouts::PosColor);
	m_D3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT stride = sizeof(VertexStruct::PosColor);
	UINT offset = 0;
	m_D3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_D3dImmediateContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	/// WVP TM등을 셋팅
	// Set constants
	XMMATRIX worldViewProj = m_World * m_View * m_Proj;
	Effects::PosColorFX->SetWorldViewProj(worldViewProj);

	// 렌더스테이트
	m_D3dImmediateContext->RSSetState(m_pRenderstate);

	// 테크닉은...
	ID3DX11EffectTechnique* mTech = Effects::PosColorFX->Tech;
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	// 랜더패스는...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, m_D3dImmediateContext);

		// 객체의 인덱스 사이즈에 맞춰서 context에 넣어
		m_D3dImmediateContext->DrawIndexed(m_IndexSize, 0, 0);
	}
}

void ObjectFactory::BuildGeometryBuffers(std::string name)
{
	//  버텍스 버퍼와 인덱스 버퍼는 이미 리소스매니저가 들고 있다. 
	GeometryBuffer* meshBuffers = m_pResourceManager->m_GBufferList.find(name)->second;
	m_IndexSize = meshBuffers->GetIndexSize();
	m_pVB = meshBuffers->GetVertexBuffer();
	m_pIB = meshBuffers->GetIndexBuffer();
}

