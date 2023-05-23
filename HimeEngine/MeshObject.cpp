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

	// 애니메이션으로 조작한 애들을 가지고 다시 mLocalTM 를 만들어줌.
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
	// 입력 배치 객체 셋팅
	m_pDeviceContext->IASetInputLayout(InputLayouts::PosNorTex);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT stride = sizeof(VertexStruct::PosNorTex);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	/// WVP TM등을 셋팅
	// Set constants
	// Set constants
	XMMATRIX view = ::XMLoadFloat4x4(&m_View);
	XMMATRIX proj = ::XMLoadFloat4x4(&m_Proj);
	XMMATRIX world = ::XMLoadFloat4x4(&m_World);
	//XMMATRIX world = XMMatrixIdentity();

	XMMATRIX worldViewProj = world * view * proj;

	// 월드의 역행렬
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

	// Set per frame constants.
	Effects::PosNorTexFX->SetDirLights(m_DirLights);
	Effects::PosNorTexFX->SetEyePosW(m_EyePosW);

	// 렌더스테이트
	m_pDeviceContext->RSSetState(m_pRenderstate);

	///TODO: 이곳에서 bool 로 RimLight를 적용할지말지 정한다. 
	// 테크닉은...
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

	// 랜더패스는...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

		// WVP는 위에서
		worldInvTranspose = MathHelper::InverseTranspose(world);

		Effects::PosNorTexFX->SetWorld(world);
		Effects::PosNorTexFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::PosNorTexFX->SetWorldViewProj(worldViewProj);
		Effects::PosNorTexFX->SetTexTransform(XMLoadFloat4x4(&m_TexTransform));
		Effects::PosNorTexFX->SetMaterial(m_Material);
		Effects::PosNorTexFX->SetDiffuseMap(m_pDiffuseMapSRV);

		_Tech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		/// GBuffer에서 온 것 (새로 만든 것)
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
	// 부모가 있는지 검사하자. 
	// 없다면
	if (myNode->m_pParentNodeInfo == nullptr)
	{
		// 내 mLocalTM를 반환
		return myNode->m_NodeData.m_LocalTM;
	}
	// 부모가 있다면 mLocalTM * mParentNode->mLocalTM를 재귀함수로 값을 가져온다. 
	return	 myNode->m_NodeData.m_LocalTM * GetHierarchyNodeTM(myNode->m_pParentNodeInfo);
}

void MeshObject::UpdateAnimation(int m_framecount)
{
	/// 1. 일단 내 상태에 따른 애니메이션을 찾는다.
	//int _type = 2; 
	int _type = m_anitype; 
	// type = 0 이면 idle / type = 1 이면 walk
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

	/// 2. 애니메이션의 모든 기준은 Tick이다.
	// Animation의 Tickcount를 나눈값(parser가 들고있음)
	int _AniTick = m_framecount * m_ticksperFrame;

	// 애니메이션이 있지만, 포지션 데이터가 없는 경우도 그냥 나온다.
	if (_nowAnimtion.m_position.size() != 0)
	{
		/// 3. Tick을 기준으로 좌/우 값을 찾는다.
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion.m_position.size(); i++)
		{
			if (_AniTick < _nowAnimtion.m_position[i]->m_time)
			{
				if (i == 0) // 애니메이션의 time이 0에서 부터 시작하는게 아닐 때 예외처리
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

		/// 4. 좌/우 인덱스를 기준으로 보간한다.
		// 현재와 다음 pos 구조체를 담는 변수
		auto _leftPos = _nowAnimtion.m_position[_leftIndex];
		auto _rightPos = _nowAnimtion.m_position[_rightIndex];

		// 현재와 다음 pos 사이의 시간이 얼마나 흘렀나를 담는 변수(0 ~ 1 사이 값으로 나타냄)
		float _NowTime = (float)_leftPos->m_time;
		float _NextTime = (float)_rightPos->m_time;

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		m_Position = XMVectorLerp(_leftPos->m_pos, _rightPos->m_pos, _floatTime);
	}

	// 애니메이션이 있지만, 회전 데이터가 없는 경우도 그냥 나온다.
	if (_nowAnimtion.m_rotation.size() != 0)
	{
		/// 3. Tick을 기준으로 좌/우 값을 찾는다.wwwwwwwwwwdawsssssss
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

		/// 4. 좌/우 인덱스를 기준으로 보간한다.
		// 현재와 다음 rot 구조체를 담는 변수
		auto _leftRot = _nowAnimtion.m_rotation[_leftIndex];
		auto _rightRot = _nowAnimtion.m_rotation[_rightIndex];

		// 현재와 다음 rot 사이의 시간이 얼마나 흘렀나를 담는 변수(0 ~ 1 사이 값으로 나타냄)
		float _NowTime = (float)_leftRot->m_time;
		float _NextTime = (float)_rightRot->m_time;

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		m_Rotate = XMQuaternionSlerp(_leftRot->m_rotQT_accumulation, _rightRot->m_rotQT_accumulation, _floatTime);
	}
}

void MeshObject::CalcLocalTM()
{
	// 애니메이션으로 조작한 애들을 가지고 다시 mLocalTM 를 만들어줌.
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

