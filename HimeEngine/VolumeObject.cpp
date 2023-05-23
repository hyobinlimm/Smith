#include "VolumeObject.h"
#include "ResourceManager.h"
#include "BuildInfo.h"
#include "HimeDevice.h"

#include "InputLayouts.h"
#include "MathHelper.h"
#include "HimeCamera.h"

VolumeObject::VolumeObject(ID3D11RasterizerState* pRS)
	:m_pRenderstate(pRS),
	m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj(),
	m_IndexCount(0)
{
	::XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();
}

VolumeObject::~VolumeObject()
{
	ReleaseCOM(m_pIB);
	ReleaseCOM(m_pVB);
	ReleaseCOM(m_pRenderstate);
}

void VolumeObject::Initialize()
{
	XMMATRIX _scale = XMMatrixScalingFromVector(m_Scale);
	XMMATRIX _rotate = XMMatrixRotationQuaternion(m_Rotate);
	XMMATRIX _position = XMMatrixTranslationFromVector(m_Position);
}

void VolumeObject::Update(HimeCamera* pCamera)
{
	XMMATRIX _scale = XMMatrixScalingFromVector(m_Scale);
	XMMATRIX _rotate = XMMatrixRotationQuaternion(m_Rotate);
	XMMATRIX _position = XMMatrixTranslationFromVector(m_Position);

	//::XMStoreFloat4x4(&m_NodeData.m_LocalTM, _scale * _rotate * _position);

	m_World = m_NodeData.m_LocalTM;
	::XMStoreFloat4x4(&m_View, pCamera->View());
	::XMStoreFloat4x4(&m_Proj, pCamera->Proj());

	m_EyePosW = XMFLOAT3(pCamera->GetPosition().x, pCamera->GetPosition().y, pCamera->GetPosition().z);
}

void VolumeObject::Render()
{
	// �Է� ��ġ ��ü ����
	m_pDeviceContext->IASetInputLayout(InputLayouts::PosColor);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// �ε������ۿ� ���ؽ����� ����
	UINT stride = sizeof(VertexStruct::PosColor);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	/// WVP TM���� ����
	// Set constants
	// Set constants
	XMMATRIX view = ::XMLoadFloat4x4(&m_View);
	XMMATRIX proj = ::XMLoadFloat4x4(&m_Proj);
	XMMATRIX world = ::XMLoadFloat4x4(&m_World);
	//XMMATRIX world = XMMatrixIdentity();

	XMMATRIX worldViewProj = world * view * proj;

	// ������ �����
	//XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

	// ����������Ʈ
	m_pDeviceContext->RSSetState(m_pRenderstate);

	ID3DX11EffectTechnique* _Tech = Effects::PosColorFX->Tech;

	D3DX11_TECHNIQUE_DESC techDesc;
	_Tech->GetDesc(&techDesc);

	// �����н���...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::PosColorFX->SetWorldViewProj(worldViewProj);

		_Tech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		/// GBuffer���� �� �� (���� ���� ��)
		m_pDeviceContext->DrawIndexed(m_IndexSize, 0, 0);
	}
}

void VolumeObject::DecomposeLocalTM()
{
	XMMATRIX _localTM = ::XMLoadFloat4x4(&m_NodeData.m_LocalTM);

	// decompose LocalTM
	XMMatrixDecompose(&m_Scale, &m_Rotate, &m_Position, _localTM);
}

void VolumeObject::SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB)
{
	m_pVB = VB;
	m_pIB = IB;
}

void VolumeObject::SetPos(XMFLOAT4 Pos)
{
	XMVECTOR _Pos = XMLoadFloat4(&Pos);

	m_Position = _Pos;
}

void VolumeObject::SetRot(XMFLOAT4 Rot)
{
	XMVECTOR _Rot = XMLoadFloat4(&Rot);

	m_Rotate = _Rot;
}