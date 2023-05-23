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
	// 애니메이션으로 조작한 애들을 가지고 다시 mLocalTM 를 만들어줌.
	XMMATRIX _scale = XMMatrixScalingFromVector(m_Scale);
	XMMATRIX _rotate = XMMatrixRotationQuaternion(m_Rotate);
	XMMATRIX _position = XMMatrixTranslationFromVector(m_Position);

	::XMStoreFloat4x4(&m_NodeData.m_LocalTM, _scale * _rotate * _position);

	m_World = GetHierarchyNodeTM(this);

	// 내가 스키니 오브젝트면 본의 nodeTM 값도 업데이트 해준다. 
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
	/// 일단 스키닝은 보이지 않도록
	//return;

	// 입력 배치 객체 셋팅
	m_pDeviceContext->IASetInputLayout(InputLayouts::PosNorTexSkin);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 렌더스테이트
	m_pDeviceContext->RSSetState(m_pRenderstate);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT stride = sizeof(VertexStruct::PosNorTexSkin);
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
	Effects::PosNorTexSkinFX->SetDirLights(m_DirLights);
	Effects::PosNorTexSkinFX->SetEyePosW(m_EyePosW);

	// 배열의 이름, 본의 개수를 넘겨주자. 
	UINT _boneCount = m_BoneList.size();
	Effects::PosNorTexSkinFX->SetBoneTransforms(m_FinalBoneMatrix, _boneCount);

	// 테크닉은...
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

	// 랜더패스는...
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

		// WVP는 위에서
		worldInvTranspose = MathHelper::InverseTranspose(world);

		Effects::PosNorTexSkinFX->SetWorld(world);
		Effects::PosNorTexSkinFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::PosNorTexSkinFX->SetWorldViewProj(worldViewProj);
		Effects::PosNorTexSkinFX->SetTexTransform(XMLoadFloat4x4(&m_TexTransform));
		Effects::PosNorTexSkinFX->SetMaterial(m_Material);
		Effects::PosNorTexSkinFX->SetDiffuseMap(m_pDiffuseMapSRV);

		_Tech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		/// GBuffer에서 온 것 (새로 만든 것)
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

void SkinningObject::DecomposeLocalTM()
{
	XMMATRIX _localTM = ::XMLoadFloat4x4(&m_NodeData.m_LocalTM);

	// decompose LoclaTM
	XMMatrixDecompose(&m_Scale, &m_Rotate, &m_Position, _localTM);
}

/*
void SkinningObject::UpdateAnimation(int m_framecount)
{
	/// 1. 일단 내 애니메이션을 찾는다.
	// 내가 애니메이션을 들고 있지 않다면 함수를 나오자. 
	//if (m_pAnimation == nullptr) return;

	Animation* _nowAnimtion = m_pAnimation;

	/// 2. 애니메이션의 모든 기준은 Tick이다.
	// Animation의 Tickcount를 나눈값(parser가 들고있음)
	int _AniTick = m_framecount * _nowAnimtion->m_ticksperFrame;

	// 애니메이션이 있지만, 포지션 데이터가 없는 경우도 그냥 나온다.
	if (_nowAnimtion->m_position.size() != 0)
	{
		/// 3. Tick을 기준으로 좌/우 값을 찾는다.
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion->m_position.size(); i++)
		{
			if (_AniTick < _nowAnimtion->m_position[i]->m_time)
			{
				if (i == 0) // 애니메이션의 time이 0에서 부터 시작하는게 아닐 때 예외처리
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

		/// 4. 좌/우 인덱스를 기준으로 보간한다.
		// 현재와 다음 pos 구조체를 담는 변수
		auto _leftPos = _nowAnimtion->m_position[_leftIndex];
		auto _rightPos = _nowAnimtion->m_position[_rightIndex];

		// 현재와 다음 pos 사이의 시간이 얼마나 흘렀나를 담는 변수(0 ~ 1 사이 값으로 나타냄)
		float _NowTime = (float)_leftPos->m_time;
		float _NextTime = (float)_rightPos->m_time;

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		m_Position = XMVectorLerp(_leftPos->m_pos, _rightPos->m_pos, _floatTime);
	}

	// 애니메이션이 있지만, 회전 데이터가 없는 경우도 그냥 나온다.
	if (_nowAnimtion->m_rotation.size() != 0)
	{
		/// 3. Tick을 기준으로 좌/우 값을 찾는다.
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

		/// 4. 좌/우 인덱스를 기준으로 보간한다.
		// 현재와 다음 rot 구조체를 담는 변수
		auto _leftRot = _nowAnimtion->m_rotation[_leftIndex];
		auto _rightRot = _nowAnimtion->m_rotation[_rightIndex];

		// 현재와 다음 rot 사이의 시간이 얼마나 흘렀나를 담는 변수(0 ~ 1 사이 값으로 나타냄)
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
	// 각 bone의 matrix를 구하자. 
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

		// 본의 offset TM은 본의 nodeTM에 스킨 메시의 worldInv를 곱해줘서 얻을 수 있다.
		_skinNodeTM = XMLoadFloat4x4(&m_NodeData.m_NodeTM);
		_skinNodeTM_inverse = XMMatrixInverse(nullptr, _skinNodeTM);

		_boneOffsetTM = _boneNodeTM * _skinNodeTM_inverse;
		_boneOffsetTM_inverse = XMMatrixInverse(nullptr, _boneOffsetTM);

		// 본의 offsetTM의 역행렬 과 본의 worldTM을 곱해주면. 각 정점에 대응되는 최종 TM이 나온다.
		// 결과 적으로 이 매트릭스는 => 'skinNode * boneNodeInverse * boneWorld' * skinWorldInverse
		_finalBoneTM = _boneOffsetTM_inverse * _boneWorldTM * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_World));

		XMFLOAT4X4 _finalTM;
		XMStoreFloat4x4(&_finalTM, _finalBoneTM);

		m_FinalBoneMatrix[i] = _finalTM;
	}
}

