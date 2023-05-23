#include "SkinningObject.h"

#include "HimeDevice.h"
#include "ResourceManager.h"
#include "BuildInfo.h"

#include "InputLayouts.h"
#include "MathHelper.h"
#include "HimeCamera.h"
#include "MeshObject.h"

SkinningObject::SkinningObject(ID3D11RasterizerState* pRS)
	: m_pRenderstate(pRS),
	m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj(),
	m_IndexCount(0),
	m_Material(),
	m_DirLights(),
	m_EyePosW(),
	m_LightCount(0),
	m_TexTransform(),
	m_PosIndex(0), m_RotIndex(0),
	m_FinalBoneMatrix()
{
	::XMStoreFloat4x4(&m_TexTransform, XMMatrixIdentity());
	::XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();
}

SkinningObject::SkinningObject(ID3D11RasterizerState* pRS, XMVECTOR pos)
	:m_pRenderstate(pRS),
	m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj(),
	m_IndexCount(0),
	m_Material(),
	m_DirLights(),
	m_EyePosW(),
	m_LightCount(0),
	m_TexTransform(),
	/*m_Position(pos), m_Rotate(), m_Scale(),*/
	m_PosIndex(0), m_RotIndex(0),
	m_FinalBoneMatrix()
{
	m_Position = pos;
	::XMStoreFloat4x4(&m_TexTransform, XMMatrixIdentity());
	::XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();
}

SkinningObject::~SkinningObject()
{	
	ReleaseCOM(m_pDiffuseMapSRV);
	ReleaseCOM(m_pIB);
	ReleaseCOM(m_pVB);
	ReleaseCOM(m_pRenderstate);
}

void SkinningObject::Initialize()
{
	m_DirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_DirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_DirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_DirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	m_DirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_Material.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_Material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_Material.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

}

void SkinningObject::Update(HimeCamera* pCamera)
{
	// �ִϸ��̼����� ������ �ֵ��� ������ �ٽ� mLocalTM �� �������.
	XMMATRIX _scale = XMMatrixScalingFromVector(m_Scale);
	XMMATRIX _rotate = XMMatrixRotationQuaternion(m_Rotate);
	XMMATRIX _position = XMMatrixTranslationFromVector(m_Position);

	::XMStoreFloat4x4(&m_NodeData.m_LocalTM, _scale * _rotate * _position);

	m_World = GetHierarchyNodeTM(this);

	// ���� ��Ű�� ������Ʈ�� ���� nodeTM ���� ������Ʈ ���ش�. 
	UpdateSkinningBoneTM();

	//XMVECTOR _scalevec = { 0.05f, 0.05f, 0.05f, 1.0f };
	//XMMATRIX _scaleMX = XMMatrixScalingFromVector(_scalevec);
	//XMFLOAT4X4 _scale4x4;
	//XMStoreFloat4x4(&_scale4x4, _scaleMX);
	//mWorld = mWorld * _scale4x4;

	//XMStoreFloat4x4(&mWorld, _tempTM);
	::XMStoreFloat4x4(&m_View, pCamera->View());
	::XMStoreFloat4x4(&m_Proj, pCamera->Proj());

	// Switch the number of lights based on key presses.
	//
	if (GetAsyncKeyState('0') & 0x8000)
		m_LightCount = 0;

	if (GetAsyncKeyState('1') & 0x8000)
		m_LightCount = 1;

	if (GetAsyncKeyState('2') & 0x8000)
		m_LightCount = 2;

	if (GetAsyncKeyState('3') & 0x8000)
		m_LightCount = 3;

	if (GetAsyncKeyState('4') & 0x8000)
		m_LightCount = 4;

	if (GetAsyncKeyState('5') & 0x8000)
		m_LightCount = 5;

	if (GetAsyncKeyState('6') & 0x8000)
		m_LightCount = 6;

	if (GetAsyncKeyState('7') & 0x8000)
		m_LightCount = 7;

	m_EyePosW = XMFLOAT3(pCamera->GetPosition().x, pCamera->GetPosition().y, pCamera->GetPosition().z);
}

void SkinningObject::Render()
{
	/// �ϴ� ��Ű���� ������ �ʵ���
	//return;

	// �Է� ��ġ ��ü ����
	m_pDeviceContext->IASetInputLayout(InputLayouts::PosNorTexSkin);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ����������Ʈ
	m_pDeviceContext->RSSetState(m_pRenderstate);

	// �ε������ۿ� ���ؽ����� ����
	UINT stride = sizeof(VertexStruct::PosNorTexSkin);
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
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

	// Set per frame constants.
	Effects::PosNorTexSkinFX->SetDirLights(m_DirLights);
	Effects::PosNorTexSkinFX->SetEyePosW(m_EyePosW);

	// �迭�� �̸�, ���� ������ �Ѱ�����. 
	UINT _boneCount = m_BoneList.size();
	Effects::PosNorTexSkinFX->SetBoneTransforms(m_FinalBoneMatrix, _boneCount);

	// ��ũ����...
	ID3DX11EffectTechnique* _Tech = Effects::PosNorTexSkinFX->Light0TexSkinTech;
	//ID3DX11EffectTechnique* _Tech = Effects::PosNorTexSkinFX->Light0TexTech;

	switch (m_LightCount)
	{
	case 1:
		_Tech = Effects::PosNorTexSkinFX->Light3TexSkinTech;
		break;
	case 2:
		_Tech = Effects::PosNorTexSkinFX->Light2TexSkinTech;
		break;
	case 3:
		_Tech = Effects::PosNorTexSkinFX->Light1TexSkinTech;
		break;
	case 4:
		_Tech = Effects::PosNorTexSkinFX->Light3TexSkinTech;
		break;
	}

	D3DX11_TECHNIQUE_DESC techDesc;
	_Tech->GetDesc(&techDesc);

	// �����н���...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

		// WVP�� ������
		worldInvTranspose = MathHelper::InverseTranspose(world);

		Effects::PosNorTexSkinFX->SetWorld(world);
		Effects::PosNorTexSkinFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::PosNorTexSkinFX->SetWorldViewProj(worldViewProj);
		Effects::PosNorTexSkinFX->SetTexTransform(XMLoadFloat4x4(&m_TexTransform));
		Effects::PosNorTexSkinFX->SetMaterial(m_Material);
		Effects::PosNorTexSkinFX->SetDiffuseMap(m_pDiffuseMapSRV);

		_Tech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		/// GBuffer���� �� �� (���� ���� ��)
		m_pDeviceContext->DrawIndexed(m_IndexSize, 0, 0);
	}
}

void SkinningObject::SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB)
{
	m_pVB = VB;
	m_pIB = IB;
}

void SkinningObject::SetTexResource(std::wstring name)
{
	wstring _texPath = m_pResourceManager->m_BuildInfoList.at(name)->GetPathMap();
	std::wstring _texturPath_wstr = std::wstring(_texPath.begin(), _texPath.end());
	const wchar_t* _texFilePath = _texturPath_wstr.c_str();

	ID3D11Resource* texResource = nullptr;
	HR(CreateDDSTextureFromFile(m_pD3dDevice,
		_texFilePath, &texResource, &m_pDiffuseMapSRV));
}

void SkinningObject::SetMaterial(ASEParser::ASEMaterial* materialData)
{
	Vector4 _ambient(materialData->m_material_ambient.x, materialData->m_material_ambient.y, materialData->m_material_ambient.z, 1.0f);
	Vector4 _diffuse(materialData->m_material_diffuse.x, materialData->m_material_diffuse.y, materialData->m_material_diffuse.z, 1.0f);
	Vector4 _Specular(materialData->m_material_specular.x, materialData->m_material_specular.y, materialData->m_material_specular.z, 1.0f);

	m_Material.Ambient = XMFLOAT4(_ambient);
	m_Material.Diffuse = XMFLOAT4(_diffuse);
	m_Material.Specular = XMFLOAT4(_Specular);
}

DirectX::XMFLOAT4X4 SkinningObject::GetHierarchyNodeTM(NodeInfo* myNode)
{
	// �θ� �ִ��� �˻�����. 
	// ���ٸ�
	if (myNode->m_pParentNodeInfo == nullptr)
	{
		// �� mLocalTM�� ��ȯ
		return myNode->m_NodeData.m_LocalTM;
	}
	// �θ� �ִٸ� mLocalTM * mParentNode->mLocalTM�� ����Լ��� ���� �����´�. 
	return	 myNode->m_NodeData.m_LocalTM * GetHierarchyNodeTM(myNode->m_pParentNodeInfo);
}

void SkinningObject::DecomposeLocalTM()
{
	XMMATRIX _localTM = ::XMLoadFloat4x4(&m_NodeData.m_LocalTM);

	// decompose LoclaTM
	XMMatrixDecompose(&m_Scale, &m_Rotate, &m_Position, _localTM);
}

/*
void SkinningObject::UpdateAnimation(int m_framecount)
{
	/// 1. �ϴ� �� �ִϸ��̼��� ã�´�.
	// ���� �ִϸ��̼��� ��� ���� �ʴٸ� �Լ��� ������. 
	//if (m_pAnimation == nullptr) return;

	Animation* _nowAnimtion = m_pAnimation;

	/// 2. �ִϸ��̼��� ��� ������ Tick�̴�.
	// Animation�� Tickcount�� ������(parser�� �������)
	int _AniTick = m_framecount * _nowAnimtion->m_ticksperFrame;

	// �ִϸ��̼��� ������, ������ �����Ͱ� ���� ��쵵 �׳� ���´�.
	if (_nowAnimtion->m_position.size() != 0)
	{
		/// 3. Tick�� �������� ��/�� ���� ã�´�.
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion->m_position.size(); i++)
		{
			if (_AniTick < _nowAnimtion->m_position[i]->m_time)
			{
				if (i == 0) // �ִϸ��̼��� time�� 0���� ���� �����ϴ°� �ƴ� �� ����ó��
				{
					_leftIndex = i;
					_rightIndex = _nowAnimtion->m_rotation.size() - 1;
				}
				else
				{
					_leftIndex = max(0, i - 1);
					_rightIndex = i;
				}
				break;
			}

			if (i == _nowAnimtion->m_position.size() - 1)
			{
				_leftIndex = i;
				_rightIndex = 0;
			}
		}

		/// 4. ��/�� �ε����� �������� �����Ѵ�.
		// ����� ���� pos ����ü�� ��� ����
		auto _leftPos = _nowAnimtion->m_position[_leftIndex];
		auto _rightPos = _nowAnimtion->m_position[_rightIndex];

		// ����� ���� pos ������ �ð��� �󸶳� �귶���� ��� ����(0 ~ 1 ���� ������ ��Ÿ��)
		float _NowTime = (float)_leftPos->m_time;
		float _NextTime = (float)_rightPos->m_time;

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		m_Position = XMVectorLerp(_leftPos->m_pos, _rightPos->m_pos, _floatTime);
	}

	// �ִϸ��̼��� ������, ȸ�� �����Ͱ� ���� ��쵵 �׳� ���´�.
	if (_nowAnimtion->m_rotation.size() != 0)
	{
		/// 3. Tick�� �������� ��/�� ���� ã�´�.
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion->m_rotation.size(); i++)
		{
			if (_AniTick < _nowAnimtion->m_rotation[i]->m_time)
			{
				if (i == 0)
				{
					_leftIndex = i;
					_rightIndex = _nowAnimtion->m_rotation.size() - 1;
				}
				else
				{
					int _num0 = i - 1;
					_leftIndex = max(0, _num0);
					_rightIndex = i;
				}
				break;
			}

			if (i == _nowAnimtion->m_rotation.size() - 1)
			{
				_leftIndex = i;
				_rightIndex = 0;
			}
		}

		/// 4. ��/�� �ε����� �������� �����Ѵ�.
		// ����� ���� rot ����ü�� ��� ����
		auto _leftRot = _nowAnimtion->m_rotation[_leftIndex];
		auto _rightRot = _nowAnimtion->m_rotation[_rightIndex];

		// ����� ���� rot ������ �ð��� �󸶳� �귶���� ��� ����(0 ~ 1 ���� ������ ��Ÿ��)
		float _NowTime = (float)_leftRot->m_time;
		float _NextTime = (float)_rightRot->m_time;

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		m_Rotate = XMQuaternionSlerp(_leftRot->m_rotQT_accumulation, _rightRot->m_rotQT_accumulation, _floatTime);

		delete _nowAnimtion;
		_nowAnimtion = nullptr;
	}
}*/

void SkinningObject::UpdateSkinningBoneTM()
{
	// �� bone�� matrix�� ������. 
	XMMATRIX _boneWorldTM;
	XMMATRIX _boneNodeTM;

	XMMATRIX _skinNodeTM;
	XMMATRIX _skinNodeTM_inverse;

	XMMATRIX _boneOffsetTM;
	XMMATRIX _boneOffsetTM_inverse;

	XMMATRIX _finalBoneTM;

	for (size_t i = 0; i < m_BoneList.size(); i++)
	{
		auto _tempBone = m_BoneList[i];

		_boneWorldTM = _tempBone->m_boneTM_WorldTM;
		_boneNodeTM = _tempBone->m_boneTM_NodeTM;

		// ���� offset TM�� ���� nodeTM�� ��Ų �޽��� worldInv�� �����༭ ���� �� �ִ�.
		_skinNodeTM = XMLoadFloat4x4(&m_NodeData.m_NodeTM);
		_skinNodeTM_inverse = XMMatrixInverse(nullptr, _skinNodeTM);

		_boneOffsetTM = _boneNodeTM * _skinNodeTM_inverse;
		_boneOffsetTM_inverse = XMMatrixInverse(nullptr, _boneOffsetTM);

		// ���� offsetTM�� ����� �� ���� worldTM�� �����ָ�. �� ������ �����Ǵ� ���� TM�� ���´�.
		// ��� ������ �� ��Ʈ������ => 'skinNode * boneNodeInverse * boneWorld' * skinWorldInverse
		_finalBoneTM = _boneOffsetTM_inverse * _boneWorldTM * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_World));

		XMFLOAT4X4 _finalTM;
		XMStoreFloat4x4(&_finalTM, _finalBoneTM);

		m_FinalBoneMatrix[i] = _finalTM;
	}
}

