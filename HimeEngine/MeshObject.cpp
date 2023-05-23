#include "HimeDevice.h"
#include "ResourceManager.h"
#include "BuildInfo.h"

#include "InputLayouts.h"
#include "MathHelper.h"
#include "HimeCamera.h"
#include "MeshObject.h"


MeshObject::MeshObject(ID3D11RasterizerState* pRS)
	:m_pRenderstate(pRS),
	m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj(),
	m_IndexCount(0),
	m_Material(),
	m_DirLights(),
	m_EyePosW(),
	m_TexTransform(),
	/*m_Position(), m_Rotate(), m_Scale(),*/
	m_PosIndex(0), m_RotIndex(0)
{
	::XMStoreFloat4x4(&m_TexTransform, XMMatrixIdentity());
	::XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();
}


MeshObject::MeshObject(ID3D11RasterizerState* pRS, XMVECTOR pos)
	:m_pRenderstate(pRS),
	m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj(),
	m_IndexCount(0),
	m_Material(),
	m_DirLights(),
	m_EyePosW(),
	m_TexTransform(),
	/*m_Position(pos), m_Rotate(), m_Scale(),*/
	m_PosIndex(0), m_RotIndex(0)
{
	m_Position = pos;
	::XMStoreFloat4x4(&m_TexTransform, XMMatrixIdentity());
	::XMStoreFloat4x4(&m_World, XMMatrixIdentity());
	
	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();
}

MeshObject::~MeshObject()
{
	ReleaseCOM(m_pDiffuseMapSRV);
	ReleaseCOM(m_pIB);
	ReleaseCOM(m_pVB);
	ReleaseCOM(m_pRenderstate);
}

void MeshObject::Initialize()
{
	m_DirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_DirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_DirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_DirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	m_DirLights[2].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_Material.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_Material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_Material.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}

void MeshObject::Update(HimeCamera* pCamera)
{
	
	XMVECTOR _DirectionScale;
	XMVECTOR _DirectionRotate;
	XMVECTOR _DirectionPosition;

	XMMATRIX _DirectionMX = XMLoadFloat4x4(&m_NodeData.m_DirectionTM);

	XMMatrixDecompose(&_DirectionScale, &_DirectionRotate, &_DirectionPosition, _DirectionMX);

	SimpleMath::Quaternion _DefaultRotation = { 0, 0.7071068, 0.7071068, 0 };

	//XMVECTOR _R = { PI / 2, PI, 0 };
	//m_MoveRot = m_MoveRot * _R;

	// �ִϸ��̼����� ������ �ֵ��� ������ �ٽ� mLocalTM �� �������.
	XMMATRIX _scale = XMMatrixScalingFromVector(m_Scale);
	XMMATRIX _defaultRotateMatrix = XMMatrixRotationQuaternion(_DefaultRotation);
	XMMATRIX _rotate = XMMatrixRotationQuaternion(m_Rotate);
	XMMATRIX _moveRotTM = XMMatrixRotationQuaternion(m_MoveRot);
	XMMATRIX _finalRotTM =  _rotate * _moveRotTM * _defaultRotateMatrix;

	XMMATRIX _position = XMMatrixTranslationFromVector(m_Position);
	
	//mPosition.vector4_f32[0] += 0.1f;
	::XMStoreFloat4x4(&m_NodeData.m_LocalTM, _scale * _rotate * _position);

	
	XMMATRIX _MoveTM = XMMatrixTranslationFromVector(m_MovePos);
	XMMATRIX _RotTM = XMMatrixRotationRollPitchYawFromVector(m_MoveRot);

	m_NodeData.m_LocalTM = m_NodeData.m_LocalTM * _RotTM * _MoveTM;
	
	//if (m_NodeName == L"Center")
	//{
	//	m_NodeData.m_LocalTM = m_NodeData.m_LocalTM + m_NodeData.m_DirectionTM;
	//}

	m_World = GetHierarchyNodeTM(this);

	/*XMMATRIX _scaleW = XMMatrixScalingFromVector({ 0.1f, 0.1f, 0.1f, 0.1f });
	XMFLOAT4X4 _scaleW4x4;
	::XMStoreFloat4x4(&_scaleW4x4, _scaleW);

	m_World = m_World * _scaleW4x4;*/

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

	m_EyePosW = XMFLOAT3(pCamera->GetPosition().x, pCamera->GetPosition().y, pCamera->GetPosition().z);
}

void MeshObject::Render()
{
	// �Է� ��ġ ��ü ����
	m_pDeviceContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �ε������ۿ� ���ؽ����� ����
	UINT stride = sizeof(VertexStruct::PosNorTex);
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
	Effects::PosNorTexFX->SetDirLights(m_DirLights);
	Effects::PosNorTexFX->SetEyePosW(m_EyePosW);

	// ����������Ʈ
	m_pDeviceContext->RSSetState(m_pRenderstate);

	///TODO: �̰����� bool �� RimLight�� ������������ ���Ѵ�. 
	// ��ũ����...
	ID3DX11EffectTechnique* _Tech = Effects::PosNorTexFX->Light2TexTech;

	switch (m_LightCount)
	{
	case 1:
		if (this->m_pDiffuseMapSRV != nullptr)
		_Tech = Effects::PosNorTexFX->Light0TexTech;
		else
		_Tech = Effects::PosNorTexFX->Light1Tech;
		break;

	case 2:
		if (this->m_pDiffuseMapSRV != nullptr)
		_Tech = Effects::PosNorTexFX->Light2TexTech;
		else
		_Tech = Effects::PosNorTexFX->Light1Tech;
		break;

	case 3:
		if (this->m_pDiffuseMapSRV != nullptr)
		_Tech = Effects::PosNorTexFX->Light3TexTech;
		else
		_Tech = Effects::PosNorTexFX->Light1Tech;
		break;

	case 4:
		if (this->m_pDiffuseMapSRV != nullptr)
		_Tech = Effects::PosNorTexFX->Light2TexRimTech;
		//else
		//_Tech = Effects::PosNorTexFX->Light2TexRimTech;
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

		Effects::PosNorTexFX->SetWorld(world);
		Effects::PosNorTexFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::PosNorTexFX->SetWorldViewProj(worldViewProj);
		Effects::PosNorTexFX->SetTexTransform(XMLoadFloat4x4(&m_TexTransform));
		Effects::PosNorTexFX->SetMaterial(m_Material);
		Effects::PosNorTexFX->SetDiffuseMap(m_pDiffuseMapSRV);

		_Tech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		/// GBuffer���� �� �� (���� ���� ��)
		m_pDeviceContext->DrawIndexed(m_IndexSize, 0, 0);
	}
}

void MeshObject::SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB)
{
	m_pVB = VB;
	m_pIB = IB;
}

void MeshObject::SetTexResource(std::wstring name)
{
	if (m_pResourceManager->m_BuildInfoList.at(name)->GetPathMap() == L"Fail") return;
	wstring _texPath = m_pResourceManager->m_BuildInfoList.at(name)->GetPathMap();
	std::wstring _texturPath_wstr = std::wstring(_texPath.begin(), _texPath.end());
	const wchar_t* _texFilePath = _texturPath_wstr.c_str();

	ID3D11Resource* texResource = nullptr;
	HR(CreateDDSTextureFromFile(m_pD3dDevice, _texFilePath, &texResource, &m_pDiffuseMapSRV));
	ReleaseCOM(texResource);
}

void MeshObject::SetMaterial(ASEParser::ASEMaterial* materialData)
{
	Vector4 _ambient(materialData->m_material_ambient.x, materialData->m_material_ambient.y, materialData->m_material_ambient.z, 1.0f);
	Vector4 _diffuse(materialData->m_material_diffuse.x, materialData->m_material_diffuse.y, materialData->m_material_diffuse.z, 1.0f);
	Vector4 _Specular(materialData->m_material_specular.x, materialData->m_material_specular.y, materialData->m_material_specular.z, 1.0f);

	m_Material.Ambient = XMFLOAT4(_ambient);
	m_Material.Diffuse = XMFLOAT4(_diffuse);
	m_Material.Specular = XMFLOAT4(_Specular);
}

DirectX::XMFLOAT4X4 MeshObject::GetHierarchyNodeTM(NodeInfo* myNode)
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

void MeshObject::UpdateAnimation(int m_framecount)
{
	/// 1. �ϴ� �� ���¿� ���� �ִϸ��̼��� ã�´�.
	//int _type = 2; 
	int _type = m_anitype; 
	// type = 0 �̸� idle / type = 1 �̸� walk
	Animation _nowAnimtion;
	if (_type == 0)
	{ 
		_nowAnimtion = m_Animation_List[_type];
	}
	else if (_type == 1) 
	{ 
		_nowAnimtion = m_Animation_List[_type]; 
	}
	else if (_type == 2)
	{
		_nowAnimtion = m_Animation_List[_type];
	}

	/// 2. �ִϸ��̼��� ��� ������ Tick�̴�.
	// Animation�� Tickcount�� ������(parser�� �������)
	int _AniTick = m_framecount * m_ticksperFrame;

	// �ִϸ��̼��� ������, ������ �����Ͱ� ���� ��쵵 �׳� ���´�.
	if (_nowAnimtion.m_position.size() != 0)
	{
		/// 3. Tick�� �������� ��/�� ���� ã�´�.
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion.m_position.size(); i++)
		{
			if (_AniTick < _nowAnimtion.m_position[i]->m_time)
			{
				if (i == 0) // �ִϸ��̼��� time�� 0���� ���� �����ϴ°� �ƴ� �� ����ó��
				{
					_leftIndex = i;
					_rightIndex = _nowAnimtion.m_position.size() - 1;
				}
				else
				{
					_leftIndex = max(0, i - 1);
					_rightIndex = i;
				}
				break;
			}

			if (i == _nowAnimtion.m_position.size() - 1)
			{
				_leftIndex = i;
				_rightIndex = 0;
			}
		}

		/// 4. ��/�� �ε����� �������� �����Ѵ�.
		// ����� ���� pos ����ü�� ��� ����
		auto _leftPos = _nowAnimtion.m_position[_leftIndex];
		auto _rightPos = _nowAnimtion.m_position[_rightIndex];

		// ����� ���� pos ������ �ð��� �󸶳� �귶���� ��� ����(0 ~ 1 ���� ������ ��Ÿ��)
		float _NowTime = (float)_leftPos->m_time;
		float _NextTime = (float)_rightPos->m_time;

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		m_Position = XMVectorLerp(_leftPos->m_pos, _rightPos->m_pos, _floatTime);
	}

	// �ִϸ��̼��� ������, ȸ�� �����Ͱ� ���� ��쵵 �׳� ���´�.
	if (_nowAnimtion.m_rotation.size() != 0)
	{
		/// 3. Tick�� �������� ��/�� ���� ã�´�.wwwwwwwwwwdawsssssss
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion.m_rotation.size(); i++)
		{
			if (_AniTick < _nowAnimtion.m_rotation[i]->m_time)
			{
				if (i == 0)
				{
					_leftIndex = i;
					_rightIndex = _nowAnimtion.m_rotation.size() - 1;
				}
				else
				{
					int _num0 = i - 1;
					_leftIndex = max(0, _num0);
					_rightIndex = i;
				}
				break;
			}

			if (i == _nowAnimtion.m_rotation.size() - 1)
			{
				_leftIndex = i;
				_rightIndex = 0;
			}
		}

		/// 4. ��/�� �ε����� �������� �����Ѵ�.
		// ����� ���� rot ����ü�� ��� ����
		auto _leftRot = _nowAnimtion.m_rotation[_leftIndex];
		auto _rightRot = _nowAnimtion.m_rotation[_rightIndex];

		// ����� ���� rot ������ �ð��� �󸶳� �귶���� ��� ����(0 ~ 1 ���� ������ ��Ÿ��)
		float _NowTime = (float)_leftRot->m_time;
		float _NextTime = (float)_rightRot->m_time;

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		m_Rotate = XMQuaternionSlerp(_leftRot->m_rotQT_accumulation, _rightRot->m_rotQT_accumulation, _floatTime);
	}
}

void MeshObject::CalcLocalTM()
{
	// �ִϸ��̼����� ������ �ֵ��� ������ �ٽ� mLocalTM �� �������.
	XMMATRIX _scale = XMMatrixScalingFromVector(m_Scale);
	XMMATRIX _rotate = XMMatrixRotationQuaternion(m_Rotate);
	XMMATRIX _position = XMMatrixTranslationFromVector(m_Position);

	//XMMATRIX _rotate = XMMatrixRotationRollPitchYawFromVector(m_Rotate);
	//XMMATRIX _scale = XMMatrixIdentity();
	//XMMATRIX _rotate = XMMatrixIdentity();
	//XMMATRIX _position = XMMatrixTranslationFromVector(m_Position);

	//mPosition.vector4_f32[0] += 0.1f;
	::XMStoreFloat4x4(&m_NodeData.m_LocalTM, _scale * _rotate * _position);
}

void MeshObject::DecomposeLocalTM()
{
	XMMATRIX _localTM = ::XMLoadFloat4x4(&m_NodeData.m_LocalTM);

	// decompose LocalTM
	XMMatrixDecompose(&m_Scale, &m_Rotate, &m_Position, _localTM);
}

void MeshObject::SetPos(XMFLOAT4 Pos)
{
	XMVECTOR _Pos = XMLoadFloat4(&Pos);

	m_Position = _Pos;
}

void MeshObject::SetRot(XMFLOAT4 Rot)
{
	XMVECTOR _Rot = XMLoadFloat4(&Rot);

	m_Rotate = _Rot;
}

