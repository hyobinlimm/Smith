#include <DirectXColors.h>
#include "InputLayouts.h"
#include "Effects.h"
#include "ResourceManager.h"
#include "HimeDevice.h"
#include "GeometryBuffer.h"
#include "BuildInfo.h"



ResourceManager* ResourceManager::m_pResourceManager = nullptr;

ResourceManager::ResourceManager()
	:m_pASEParser(nullptr),
	m_IndexCount(0)
{
	/// DirthGraphics를 싱글톤으로 만들어야 할 것 같다. 
	m_D3dDevice = HimeDevice::GetInst()->GetD3dDevice();
}

ResourceManager::~ResourceManager()
{
	if (m_NodeInfoList.size() != 0)
	{
		for (auto _node : m_NodeInfoList)
		{
			delete _node.second->m_pParentNodeInfo;
			_node.second->m_pParentNodeInfo = nullptr;

			delete _node.second->m_pChildNodeInfo;
			_node.second->m_pChildNodeInfo = nullptr;

			delete _node.second->m_pAnimation;
			_node.second->m_pAnimation = nullptr;

			if (_node.second->m_BoneList.size() != 0)
			{
				for (auto _bone : _node.second->m_BoneList)
				{
					delete _bone;
				}
			}

			delete _node.second;
		}
		m_NodeInfoList.clear();
	}

	for (auto _info : m_BuildInfoList)
	{
		delete _info.second;
		_info.second = nullptr;
	}
	m_BuildInfoList.clear();

	for (auto _gBuffer : m_GBufferList)
	{
		delete _gBuffer.second;
		_gBuffer.second = nullptr;
	}
	m_BuildInfoList.clear();


	SafeDelete(m_pASEParser);

	InputLayouts::DestroyAll();
	Effects::DestroyAll();
}

ResourceManager* ResourceManager::GetInst()
{
	if (!m_pResourceManager)
	{
		m_pResourceManager = new ResourceManager();
	}

	return m_pResourceManager;
}

void ResourceManager::DestroyInst()
{
	if (!m_pResourceManager)
	{
		return;
	}

	delete m_pResourceManager;
	m_pResourceManager = nullptr;
}

void ResourceManager::Initialize()
{
	Effects::InitAll();
	InputLayouts::InitAll();

	m_pASEParser = new CASEParser();
	m_pASEParser->Init();

	Create_AxisGeometry();
	Create_GridGeometry();
}

GeometryBuffer* ResourceManager::GetGBuffer(std::wstring name)
{
	// GBufferList에 내가 원하는 key를 가지고 있는지 검색
	std::map <std::wstring, GeometryBuffer*> ::iterator it;

	it = m_GBufferList.find(name);

	// GBufferList에 있다면 그 값을 return
	return it->second;
}

GeometryBuffer* ResourceManager::GetVBuffer(std::wstring name)
{
	// GBufferList에 내가 원하는 key를 가지고 있는지 검색
	std::map <std::wstring, GeometryBuffer*> ::iterator it;

	it = m_VBufferList.find(name);

	if (it != m_VBufferList.end())
	{
		// GBufferList에 있다면 그 값을 return
		return it->second;
	}
	return nullptr;
}

void ResourceManager::CreateBuildInfo(std::wstring name, std::wstring ASEName, std::wstring mapName)
{
	BuildInfo* _newInfo = new BuildInfo();

	_newInfo->SetName(name.c_str());
	_newInfo->SetPathASE(ASEName.c_str());
	_newInfo->SetPathMap(mapName.c_str());
	m_BuildInfoList.insert(make_pair(_newInfo->GetName().c_str(), _newInfo));
}

void ResourceManager::SetBuildInfoAll(std::vector<FilePath*> pFilePath)
{
	// ASE, DDS 파일경로를 모두 들고 있게 한다. 
	for (auto _path : pFilePath)
	{
		CreateBuildInfo(_path->ID_name.c_str(), _path->FileName.c_str(), _path->TexName.c_str());
	}
}

/// <summary>
/// ASE파일 이름으로 로드하는 함수
/// </summary>
void ResourceManager::LoadASEObject(std::wstring name)
{
	string _name;
	_name.assign(name.begin(), name.end());

	m_pASEParser->Load((LPSTR)_name.c_str());

	UINT _loadObjCount = m_pASEParser->m_MeshList.size();

	if (_loadObjCount == 0) return;

	// parser로 읽어 온 meshData 최적화
	for (UINT i = 0; i < _loadObjCount; i++)
	{
		m_pASEParser->Convert_NewOptimize(m_pASEParser->GetMesh(i));
	}

	// 만약 이미 이름을 담는 벡터에 이전 값이 들어있다면 한번 초기화해주고 다시 이름을 담아주자. 
	if (m_NodeInfoList.size() != 0)
	{
		m_NodeInfoList.clear();
	}

	// ASE Parser 읽어 온 데이터를 담을 object 객체 생성. 
	for (UINT i = 0; i < _loadObjCount; i++)
	{
		ASEParser::GeomObject* _newGeometryObject = m_pASEParser->GetMesh(i);

		wstring _newGeoName;
		_newGeoName.assign(_newGeometryObject->m_nodename.begin(), _newGeometryObject->m_nodename.end());

		switch (_newGeometryObject->m_type)
		{
		case eGeomobject:
		{
			if (_newGeometryObject->m_is_skinningobject == false)
			{
				Create_MeshGeometry(_newGeoName, _newGeometryObject);
				// collision 위해서 최재영이 만듬.
				if (_loadObjCount == 33 && _newGeoName == L"Center")
					Create_Volume(_newGeoName, _newGeometryObject);
				if (_loadObjCount == 1)
					Create_Volume(_newGeoName, _newGeometryObject);
				Create_NodeInfo_MeshObject(_newGeometryObject);
			}
			else
			{
				Create_SkinningMeshGeometry(_newGeoName, _newGeometryObject);
				// collision 위해서 최재영이 만듬.
				if (_name == "smith" && _newGeoName == L"Center")
					Create_Volume(_newGeoName, _newGeometryObject);
				if (i == 0 && _newGeoName != L"smith")
					Create_Volume(_newGeoName, _newGeometryObject);
				Create_NodeInfo_SkinningObject(_newGeometryObject);
			}

			/// 애니메이션이 있다면 넣어주는 함수. 
			if (_newGeometryObject->m_isAnimated == true) // 용사 애니메이션 총 33개
			{
				Create_Animation(m_pASEParser, _newGeoName);
				m_NodeInfoList[_newGeoName]->m_LastFrame = m_pASEParser->m_scenedata.m_lastframe;
				m_NodeInfoList[_newGeoName]->m_ticksperFrame = m_pASEParser->m_scenedata.m_ticksperframe;
			}
		}
		break;

		case eHelperObject:
			break;
		case eShape:
			break;

		default:
			break;
		}
	}

	m_pASEParser->ClearASEParser();
	//Delete_ASEParser();
}

/// <summary>
/// 외부에서 넣어준 BuildInfo의 IDName을 가지고 로드하는 함수
/// </summary>
void ResourceManager::LoadASEObject_BuildInfo(std::wstring IDName)
{
	LoadASEObject(m_BuildInfoList[IDName.c_str()]->GetPathASE());
}

void ResourceManager::Create_AxisGeometry()
{
	GeometryBuffer* _newGBuffer = new GeometryBuffer();

	_newGBuffer->m_Name = L"Axis";

	ID3D11Buffer* m_VB;
	ID3D11Buffer* m_IB;

	// 정점 버퍼를 생성 
	// 각 축에 맞도록 6개의 정점을 만듦.
	VertexStruct::PosColor vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Red)  },	// x축 (빨강)
		{ XMFLOAT3(10.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Red)  },

		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Green)},	// y축 (초록)
		{ XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT4((const float*)&Colors::Green)},

		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Blue)	},	// z축 (파랑)
		{ XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT4((const float*)&Colors::Blue) }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexStruct::PosColor) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(m_D3dDevice->CreateBuffer(&vbd, &vinitData, &m_VB));

	_newGBuffer->SetVertexBuffer(m_VB);
	_newGBuffer->SetVertexSize(6);

	// 인덱스 버퍼를 생성
	// 3개의 축을 나타냄.
	UINT indices[] = {
		// x축
		0, 1,

		// y축
		2, 3,

		// z축
		4, 5,
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(m_D3dDevice->CreateBuffer(&ibd, &iinitData, &m_IB));

	_newGBuffer->SetIndexBuffer(m_IB);
	_newGBuffer->SetIndexSize(6);

	// 생성된 vertex, index Buffer를 map넣어줌. 
	// mesh를 생성 후 mMeshList로만 접근할 것이기 때문에 이동생성자를 이용함. 
	m_GBufferList.insert(make_pair(_newGBuffer->m_Name, std::move(_newGBuffer)));
}

void ResourceManager::Create_GridGeometry()
{
	GeometryBuffer* _newGBuffer = new GeometryBuffer();

	_newGBuffer->m_Name = L"Grid";

	ID3D11Buffer* _VB;
	ID3D11Buffer* _IB;

	// 정점 버퍼를 생성
	// 40개의 정점을 만듦
	VertexStruct::PosColor vertices[100];
	for (int i = 0; i < 100; i++)
	{
		vertices[i].Pos = XMFLOAT3((float)(i % 10) - 5.0f, 0.0f, (float)(i / 10) - 5.0f);
		vertices[i].Color = XMFLOAT4((const float*)&Colors::DarkGray);	// (어두운 회색)
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexStruct::PosColor) * 100;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(m_D3dDevice->CreateBuffer(&vbd, &vinitData, &_VB));

	_newGBuffer->SetVertexBuffer(_VB);
	_newGBuffer->SetVertexSize(100);

	// 인덱스 버퍼를 생성
	// 40개의 라인을 나타냄
	UINT indices[40];
	for (int i = 0; i < 10; i++)
	{
		indices[i * 2] = i;
		indices[i * 2 + 1] = i + 90;
	}

	for (int i = 0; i < 10; i++)
	{
		indices[20 + (i * 2)] = i * 10;
		indices[20 + (i * 2) + 1] = i * 10 + 9;
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 40;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB));

	_newGBuffer->SetIndexBuffer(_IB);
	_newGBuffer->SetIndexSize(40);

	// 생성된 vertex, index Buffer를 map넣어줌. 
	// mesh를 생성 후 mMeshList로만 접근할 것이기 때문에 이동생성자를 이용함. 
	m_GBufferList.insert(make_pair(_newGBuffer->m_Name, std::move(_newGBuffer)));
}

/// <summary>
/// 메시오브젝트의 기하정보를 만든다. 
/// </summary>
void ResourceManager::Create_MeshGeometry(std::wstring name, ASEParser::GeomObject* meshData)
{
	UINT _vcount = 0;
	UINT _tcount = 0;

	_vcount = meshData->m_opt_vertex.size();
	std::vector<VertexStruct::PosNorTex> vertices(_vcount);

	/// TODO:REMOVE - 헬퍼오브젝트가 들어오면 밑에 부분을 타지 않고 함수를 빠져나온다.
	if (meshData->m_type == eHelperObject) return;
	if (meshData->m_type == eShape) return;

	// nodeTM의 역행렬을 만들자. 
	XMFLOAT4X4 _nodeTM = meshData->m_nodeTM;
	XMMATRIX _tempTM = XMLoadFloat4x4(&_nodeTM);
	XMMATRIX _inverseNodeTM = XMMatrixInverse(nullptr, _tempTM);
	//XMMATRIX _inverseNodeTM = CreateInverseNodeTM(*meshData);

	for (size_t i = 0; i < _vcount; i++)
	{
		// 현재 vertex pos를 xmvector에 넣어주자. 
		XMVECTOR _nowVectexPos{ meshData->m_opt_vertex[i]->m_pos.x ,
		meshData->m_opt_vertex[i]->m_pos.y,
		meshData->m_opt_vertex[i]->m_pos.z, 1.f };

		/// 신성현의 ASEExport를 사용할떄, static mesh는 inverseNodeTM 부분을 안해줘도 된다. 
		/// 왜냐하면 이미 vertex는 localTM으로 빠지기 때문이다.
		///


		/// 효빈 버전
		/*
		// 현재 vertex pos * nodeTM의 역행렬을 곱하자. (local space에서의 좌표를 구하기 위해)
		//XMVECTOR _localPos = XMVector4Transform(_nowVectexPos, _inverseNodeTM);
		XMVECTOR _localPos = XMVector3Transform(_nowVectexPos, _inverseNodeTM);

		// _localPos 를 vertices의 pos에 넣어주자. 
		XMStoreFloat3(&vertices[i].Pos, _localPos);
		*/


		/// Local Space로 보내는 버전 - SimpleMath사용
		Vector4 _pos;
		_pos.x = meshData->m_opt_vertex[i]->m_pos.x;
		_pos.y = meshData->m_opt_vertex[i]->m_pos.y;
		_pos.z = meshData->m_opt_vertex[i]->m_pos.z;
		_pos.w = 1.0f;

		Vector4 _result;
		Vector4::Transform(_pos, _inverseNodeTM, _result);

		vertices[i].Pos.x = _result.x;
		vertices[i].Pos.y = _result.y;
		vertices[i].Pos.z = _result.z;

		/// 그냥 복사하는 버전
		//vertices[i].Pos.x = meshData->m_opt_vertex[i]->m_pos.x;
		//vertices[i].Pos.y = meshData->m_opt_vertex[i]->m_pos.y;
		//vertices[i].Pos.z = meshData->m_opt_vertex[i]->m_pos.z;
		

		vertices[i].Normal.x = meshData->m_opt_vertex[i]->m_normal.x;
		vertices[i].Normal.y = meshData->m_opt_vertex[i]->m_normal.y;
		vertices[i].Normal.z = meshData->m_opt_vertex[i]->m_normal.z;


		vertices[i].Normal.x = meshData->m_opt_vertex[i]->m_normal.x;
		vertices[i].Normal.y = meshData->m_opt_vertex[i]->m_normal.y;
		vertices[i].Normal.z = meshData->m_opt_vertex[i]->m_normal.z;

		vertices[i].Tex.x = meshData->m_opt_vertex[i]->u;
		vertices[i].Tex.y = meshData->m_opt_vertex[i]->v;
	}

	_tcount = meshData->m_mesh_numfaces;

	m_IndexCount = 3 * _tcount;
	std::vector<UINT> indices(m_IndexCount);
	for (UINT i = 0; i < _tcount; ++i)
	{
		indices[i * 3 + 0] = meshData->m_opt_index[i].index[0];
		indices[i * 3 + 1] = meshData->m_opt_index[i].index[2];
		indices[i * 3 + 2] = meshData->m_opt_index[i].index[1];
	}

	// 이제 버텍스 버퍼와 인덱스 버퍼를 만들어서 기하버퍼로 넣는다. 
	GeometryBuffer* _newGBuffer = new GeometryBuffer();
	_newGBuffer->m_Name = name;

	ID3D11Buffer* _VB;
	ID3D11Buffer* _IB;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexStruct::PosNorTex) * _vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	(m_D3dDevice->CreateBuffer(&vbd, &vinitData, &_VB));

	_newGBuffer->SetVertexBuffer(_VB);
	_newGBuffer->SetVertexSize(_vcount);

	// Pack the indices of all the meshes into one index buffer.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB));

	_newGBuffer->SetIndexBuffer(_IB);
	_newGBuffer->SetIndexSize(m_IndexCount);


	/// 만들어진 기하버퍼를 리소스 매니저 안에 맵으로 가지고 있는 것.
	// mesh를 생성 후 mMeshList로만 접근할 것이기 때문에 이동생성자를 이용함. 
	// 버퍼들을 담은 벡터를 받아서 넣어준다. 	
	m_GBufferList.insert(make_pair(name, std::move(_newGBuffer)));
}

void ResourceManager::Create_NodeInfo_MeshObject(ASEParser::GeomObject* meshData)
{
	wstring _geoName;
	_geoName.assign(meshData->m_nodename.begin(), meshData->m_nodename.end());
	wstring _geoParentName;
	_geoParentName.assign(meshData->m_nodeparent.begin(), meshData->m_nodeparent.end());

	NodeInfo* _newNodeInfo = new NodeInfo();

	// nodeInfo에 이름과 TM를 담아서 간다. 
	_newNodeInfo->m_type = (int)meshData->m_type;
	_newNodeInfo->m_NodeName = _geoName;
	_newNodeInfo->m_ParentName = _geoParentName;
	_newNodeInfo->m_NodeData.m_NodeTM = CreateNodeTM(*meshData);
	_newNodeInfo->m_IsSkinningObj = meshData->m_is_skinningobject;

	m_NodeInfoList.insert(make_pair(_newNodeInfo->m_NodeName, _newNodeInfo));
}

void ResourceManager::Create_SkinningMeshGeometry(std::wstring name, ASEParser::GeomObject* meshData)
{
	/// 특정 형태의 기하버퍼를 만든다. 

	UINT vcount = 0;
	UINT tcount = 0;

	vcount = meshData->m_opt_vertex.size();
	std::vector<VertexStruct::PosNorTexSkin> vertices(vcount);

	// 헬퍼오브젝트가 들어오면 밑에 부분을 타지 않고 함수를 빠져나온다.
	if (meshData->m_type == eHelperObject) return;
	if (meshData->m_type == eShape) return;

	// nodeTM의 역행렬을 만들자.
	XMFLOAT4X4 _nodeTM = meshData->m_nodeTM;
	XMMATRIX _tempTM = XMLoadFloat4x4(&_nodeTM);
	XMMATRIX _inverseNodeTM = XMMatrixInverse(nullptr, _tempTM);
	//XMMATRIX _inverseNodeTM = CreateInverseNodeTM(*meshData);

	for (size_t i = 0; i < vcount; i++)
	{
		// 현재 vertex pos를 xmvector에 넣어주자. 
		XMVECTOR _nowVectexPos{ meshData->m_opt_vertex[i]->m_pos.x ,
		meshData->m_opt_vertex[i]->m_pos.y,
		meshData->m_opt_vertex[i]->m_pos.z, 1.f };

		// 현재 vertex pos * nodeTM의 역행렬을 곱하자. (local space에서의 좌표를 구하기 위해)
		XMVECTOR _localPos = XMVector4Transform(_nowVectexPos, _inverseNodeTM);
		//XMVECTOR _localPos = _nowVectexPos;

		// _localPos 를 vertices의 pos에 넣어주자. 
		XMStoreFloat3(&vertices[i].Pos, _localPos);

		vertices[i].Normal.x = meshData->m_opt_vertex[i]->m_normal.x;
		vertices[i].Normal.y = meshData->m_opt_vertex[i]->m_normal.y;
		vertices[i].Normal.z = meshData->m_opt_vertex[i]->m_normal.z;

		vertices[i].Tex.x = meshData->m_opt_vertex[i]->u;
		vertices[i].Tex.y = meshData->m_opt_vertex[i]->v;

		// ==================== Skinnig 추가 코드 ====================
		vertices[i].Weights1.x = meshData->m_opt_vertex[i]->m_bw1.second;
		vertices[i].Weights1.y = meshData->m_opt_vertex[i]->m_bw2.second;
		vertices[i].Weights1.z = meshData->m_opt_vertex[i]->m_bw3.second;
		vertices[i].Weights1.w = meshData->m_opt_vertex[i]->m_bw4.second;
		vertices[i].Weights2.x = meshData->m_opt_vertex[i]->m_bw5.second;
		vertices[i].Weights2.y = meshData->m_opt_vertex[i]->m_bw6.second;
		vertices[i].Weights2.z = meshData->m_opt_vertex[i]->m_bw7.second;
		vertices[i].Weights2.w = meshData->m_opt_vertex[i]->m_bw8.second;
		vertices[i].Weights3.x = meshData->m_opt_vertex[i]->m_bw9.second;
		vertices[i].Weights3.y = meshData->m_opt_vertex[i]->m_bw9.second;
		vertices[i].Weights3.z = meshData->m_opt_vertex[i]->m_bw9.second;
		vertices[i].Weights3.w = meshData->m_opt_vertex[i]->m_bw9.second;
		vertices[i].BoneIndices[0] = meshData->m_opt_vertex[i]->m_bw1.first;
		vertices[i].BoneIndices[1] = meshData->m_opt_vertex[i]->m_bw2.first;
		vertices[i].BoneIndices[2] = meshData->m_opt_vertex[i]->m_bw3.first;
		vertices[i].BoneIndices[3] = meshData->m_opt_vertex[i]->m_bw4.first;
		vertices[i].BoneIndices[4] = meshData->m_opt_vertex[i]->m_bw5.first;
		vertices[i].BoneIndices[5] = meshData->m_opt_vertex[i]->m_bw6.first;
		vertices[i].BoneIndices[6] = meshData->m_opt_vertex[i]->m_bw7.first;
		vertices[i].BoneIndices[7] = meshData->m_opt_vertex[i]->m_bw8.first;
		vertices[i].BoneIndices[8] = meshData->m_opt_vertex[i]->m_bw9.first;
	}

	tcount = meshData->m_mesh_numfaces;

	m_IndexCount = 3 * tcount;
	std::vector<UINT> indices(m_IndexCount);
	for (UINT i = 0; i < tcount; ++i)
	{
		indices[i * 3 + 0] = meshData->m_opt_index[i].index[0];
		indices[i * 3 + 1] = meshData->m_opt_index[i].index[2];
		indices[i * 3 + 2] = meshData->m_opt_index[i].index[1];
	}

	ID3D11Buffer* _VB;
	ID3D11Buffer* _IB;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexStruct::PosNorTexSkin) * vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	(m_D3dDevice->CreateBuffer(&vbd, &vinitData, &_VB));

	GeometryBuffer* _newGBuffer = new GeometryBuffer();

	_newGBuffer->m_Name = name;

	_newGBuffer->SetVertexBuffer(_VB);
	_newGBuffer->SetVertexSize(vcount);

	// Pack the indices of all the meshes into one index buffer.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB));

	_newGBuffer->SetIndexBuffer(_IB);
	_newGBuffer->SetIndexSize(m_IndexCount);


	/// 만들어진 기하버퍼를 리소스 매니저 안에 맵으로 가지고 있는 것.
	// 버퍼들을 담은 벡터를 받아서 넣어준다. 	
	m_GBufferList.insert(make_pair(name, _newGBuffer));
}

/// <summary>
/// 스키닝 오브젝트의 nodeInfo를 만들어준다. 
/// nodeTM, 부모-자식 관계(이름), bone 정보
/// </summary>
/// <param name="meshData"></param>
void ResourceManager::Create_NodeInfo_SkinningObject(ASEParser::GeomObject* meshData)
{
	NodeInfo* _newNodeInfo = new NodeInfo();

	// 내가 skinning mesh인지 아닌지 검사하자. 
	if (meshData->m_vector_bone_list.size() != 0)
	{
		_newNodeInfo->m_IsSkinningObj = true;

		// nodeINfo 구조체에 담아가자.
		for (int i = 0; i < meshData->m_vector_bone_list.size(); i++)
		{
			Bone* _CopyBone = new Bone();
			_CopyBone->m_bone_name = meshData->m_vector_bone_list[i]->m_bone_name;
			_CopyBone->m_bone_number = meshData->m_vector_bone_list[i]->m_bone_number;
			_CopyBone->m_boneTM_NodeTM = meshData->m_vector_bone_list[i]->m_boneTM_NodeTM;
			_CopyBone->m_boneTM_WorldTM = meshData->m_vector_bone_list[i]->m_boneTM_WorldTM;

			_newNodeInfo->m_BoneList.push_back(_CopyBone);
		}
	}

	wstring _geoNodeName;
	_geoNodeName.assign(meshData->m_nodename.begin(), meshData->m_nodename.end());

	wstring _geoParentName;
	_geoParentName.assign(meshData->m_nodeparent.begin(), meshData->m_nodeparent.end());

	// nodeInfo에 이름과 TM를 담아서 간다. 
	_newNodeInfo->m_type = (int)meshData->m_type;
	_newNodeInfo->m_NodeName = _geoNodeName;
	_newNodeInfo->m_ParentName = _geoParentName;
	_newNodeInfo->m_NodeData.m_NodeTM = CreateNodeTM(*meshData);
	_newNodeInfo->m_IsSkinningObj = meshData->m_is_skinningobject;

	m_NodeInfoList.insert(make_pair(_newNodeInfo->m_NodeName, _newNodeInfo));
}

void ResourceManager::Create_HelperGeometry(std::wstring name, int meshCount, ASEParser::GeomObject* meshData)
{

}

void ResourceManager::Create_NodeInfo_Hepler(std::wstring name, int meshCount, ASEParser::GeomObject* meshData)
{

}

void ResourceManager::Create_ShapeGeometry(ASEParser::GeomObject* meshData)
{

}

void ResourceManager::Create_NodeInfo_Shape(ASEParser::GeomObject* meshData)
{

}

void ResourceManager::Create_Animation(CASEParser* pASEparser, std::wstring name)
{
	//애니메이션 상태가 2개다. 그러므로 2개를 다 찾으면 FOR문을 탈출하자. 
	static UINT _findAll = 0; 

	for (auto _targetAni : pASEparser->m_list_animation)
	{
		wstring _targetName;
		_targetName.assign(_targetAni->m_nodename.begin(), _targetAni->m_nodename.end());

		if (name == _targetName)
		{
			m_NodeInfoList[name]->m_pAnimation = new Animation();

			m_NodeInfoList[name]->m_pAnimation->m_nodename = _targetAni->m_nodename;
			m_NodeInfoList[name]->m_pAnimation->m_typename = _targetAni->m_typename;
			m_NodeInfoList[name]->m_pAnimation->m_position = _targetAni->m_position;
			m_NodeInfoList[name]->m_pAnimation->m_rotation = _targetAni->m_rotation;
			m_NodeInfoList[name]->m_pAnimation->m_scale = _targetAni->m_scale;
			m_NodeInfoList[name]->m_pAnimation->m_minKeyframe = _targetAni->m_minKeyframe;

			m_NodeInfoList[name]->m_Animation_List.push_back(*m_NodeInfoList[name]->m_pAnimation);

			int size = m_NodeInfoList[name]->m_Animation_List.size();

			++_findAll; 
		}

		// 애니메이션 정보를 다 찾았따면 탈출하자. 
		if (_findAll == 3)
		{
			_findAll = 0;
			break;
		}
	}
}

DirectX::XMFLOAT4X4 ResourceManager::CreateNodeTM(ASEParser::GeomObject& meshData)
{
	XMFLOAT4X4 _nodeTM;

	_nodeTM._11 = meshData.m_tm_row0.x;
	_nodeTM._12 = meshData.m_tm_row0.y;
	_nodeTM._13 = meshData.m_tm_row0.z;
	_nodeTM._14 = 0.f;

	_nodeTM._21 = meshData.m_tm_row2.x;
	_nodeTM._22 = meshData.m_tm_row2.y;
	_nodeTM._23 = meshData.m_tm_row2.z;
	_nodeTM._24 = 0.f;

	_nodeTM._31 = meshData.m_tm_row1.x;
	_nodeTM._32 = meshData.m_tm_row1.y;
	_nodeTM._33 = meshData.m_tm_row1.z;
	_nodeTM._34 = 0.f;

	_nodeTM._41 = meshData.m_tm_row3.x;
	_nodeTM._42 = meshData.m_tm_row3.y;
	_nodeTM._43 = meshData.m_tm_row3.z;
	_nodeTM._44 = 1.f;

	return _nodeTM;
}

DirectX::XMMATRIX ResourceManager::CreateInverseNodeTM(ASEParser::GeomObject& meshData)
{
	XMFLOAT4X4 _nodeTM = CreateNodeTM(meshData);

	XMMATRIX _NodeTM = XMLoadFloat4x4(&_nodeTM);
	XMMATRIX _inverseNodeTM = XMMatrixInverse(nullptr, _NodeTM);

	return _inverseNodeTM;
}

void ResourceManager::Delete_ASEParser()
{

	if (m_pASEParser->m_list_materialdata.size() != 0)
	{
		/*for (auto _nowMat : ASEparser.GetMaterialList())
		{
			if (_nowMat->m_map_diffuse != nullptr)
			{
				delete _nowMat->m_map_diffuse;
				_nowMat->m_map_diffuse = nullptr;
			}

			delete _nowMat;
		}*/
		m_pASEParser->m_list_materialdata.clear();
	}

	if (m_pASEParser->m_MeshList.size() != 0)
	{
		for (auto _obj : m_pASEParser->m_MeshList)
		{
			if (_obj->m_vector_wvertexs.size() != 0)
			{
				for (auto _wvertex : _obj->m_vector_wvertexs)
				{
					delete _wvertex;
				}
			}

			if (_obj->m_vector_bone_list.size() != 0)
			{
				for (auto _bone : _obj->m_vector_bone_list)
				{
					delete _bone;
				}
			}

			if (_obj->m_list_lightanimation.size() != 0)
			{
				for (auto _lightanimation : _obj->m_list_lightanimation)
				{
					delete _lightanimation;
				}
			}

			if (_obj->m_vector_shape_line.size() != 0)
			{
				for (auto _line : _obj->m_vector_shape_line)
				{
					delete _line;
				}
			}

			if (_obj->m_opt_vertex.size() != 0)
			{
				for (auto _vertex : _obj->m_opt_vertex)
				{
					delete _vertex;
				}
			}

			if (_obj->m_mesh_tvertex.size() != 0)
			{
				for (auto _tvertex : _obj->m_mesh_tvertex)
				{
					delete _tvertex;
				}
			}

			if (_obj->m_meshface.size() != 0)
			{
				for (auto _face : _obj->m_meshface)
				{
					delete _face;
				}
			}

			if (_obj->m_meshvertex.size() != 0)
			{
				for (auto _vectex : _obj->m_meshvertex)
				{
					delete _vectex;
				}
			}

			delete _obj;
		}

		m_pASEParser->m_MeshList.clear();
	}

	if (m_pASEParser->m_list_animation.size() != 0)
	{
		for (auto _ani : m_pASEParser->m_list_animation)
		{
			delete _ani;
		}

		m_pASEParser->m_list_animation.clear();
	}
}

void ResourceManager::Create_Volume(std::wstring name, ASEParser::GeomObject* meshData)
{
	if (name == L"wall_l_tex")
	{
		UINT _vcount = 8;
		UINT _tcount = 26;

		float minX1 = 0;
		float minX2 = 0;
		float minY = 0;
		float minZ1 = 0;
		float minZ2 = 0;
		float maxX1 = 0;
		float maxX2 = 0;
		float maxY = 0;
		float maxZ1 = 0;
		float maxZ2 = 0;

		for (size_t i = 0; i < meshData->m_opt_vertex.size(); i++)
		{
			if (minX1 == 0)
				minX1 = meshData->m_opt_vertex[i]->m_pos.x;
			if (minY == 0)
				minY = meshData->m_opt_vertex[i]->m_pos.y;
			if (minZ1 == 0)
				minZ1 = meshData->m_opt_vertex[i]->m_pos.z;
			if (maxX2 == 0)
				maxX2 = meshData->m_opt_vertex[i]->m_pos.x;
			if (maxY == 0)
				maxY = meshData->m_opt_vertex[i]->m_pos.y;
			if (maxZ2 == 0)
				maxZ2 = meshData->m_opt_vertex[i]->m_pos.z;

			minX1 = min(minX1, meshData->m_opt_vertex[i]->m_pos.x);
			minY = min(minY, meshData->m_opt_vertex[i]->m_pos.y);
			minZ1 = min(minZ1, meshData->m_opt_vertex[i]->m_pos.z);
			maxX2 = max(maxX2, meshData->m_opt_vertex[i]->m_pos.x);
			maxY = max(maxY, meshData->m_opt_vertex[i]->m_pos.y);
			maxZ2 = max(maxZ2, meshData->m_opt_vertex[i]->m_pos.z);
		}

		minX1 += 0.2f;
		minX2 = minX1 + 3.1f;
		minY += 0.2f;
		minZ1 += 0.2f;
		minZ2 = maxZ2 - 3.f;
		maxX1 = minX1 + 2.9f;
		maxX2 -= 0.2f;
		maxY -= 0.2f;
		maxZ1 = maxZ2 - 0.2f;
		maxZ2 -= 0.2f;

		VertexStruct::PosColor vertices1[] =
		{
			{ XMFLOAT3(maxX1, minY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX1, maxY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX1, maxY, minZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX1, minY, minZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, minY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, maxY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, maxY, minZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, minY, minZ1), XMFLOAT4((const float*)&Colors::Green)  }
		};

		VertexStruct::PosColor vertices2[] =
		{
			{ XMFLOAT3(maxX2, minY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX2, maxY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX2, maxY, minZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX2, minY, minZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, minY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, maxY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, maxY, minZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, minY, minZ2), XMFLOAT4((const float*)&Colors::Green)  }
		};

		GeometryBuffer* _newVBuffer1 = new GeometryBuffer();
		GeometryBuffer* _newVBuffer2 = new GeometryBuffer();
		_newVBuffer1->m_Name = name;
		_newVBuffer2->m_Name = name;

		ID3D11Buffer* _VB1;
		ID3D11Buffer* _IB1;

		ID3D11Buffer* _VB2;
		ID3D11Buffer* _IB2;

		D3D11_BUFFER_DESC vbd1;
		vbd1.Usage = D3D11_USAGE_IMMUTABLE;
		vbd1.ByteWidth = sizeof(VertexStruct::PosColor) * 8;
		vbd1.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd1.CPUAccessFlags = 0;
		vbd1.MiscFlags = 0;
		vbd1.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData1;
		vinitData1.pSysMem = vertices1;
		m_D3dDevice->CreateBuffer(&vbd1, &vinitData1, &_VB1);

		_newVBuffer1->SetVertexBuffer(_VB1);
		_newVBuffer1->SetVertexSize(_vcount);

		D3D11_BUFFER_DESC vbd2;
		vbd2.Usage = D3D11_USAGE_IMMUTABLE;
		vbd2.ByteWidth = sizeof(VertexStruct::PosColor) * 8;
		vbd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd2.CPUAccessFlags = 0;
		vbd2.MiscFlags = 0;
		vbd2.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData2;
		vinitData2.pSysMem = vertices2;
		m_D3dDevice->CreateBuffer(&vbd2, &vinitData2, &_VB2);

		_newVBuffer2->SetVertexBuffer(_VB2);
		_newVBuffer2->SetVertexSize(_vcount);

		UINT indices[] =
		{
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			1, 5,
			5, 4,
			4, 0,
			5, 6,
			6, 7,
			7, 4,
			4, 5,
			2, 6,
			7, 3
		};

		m_IndexCount = _tcount;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
		m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB1);
		m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB2);

		_newVBuffer1->SetIndexBuffer(_IB1);
		_newVBuffer1->SetIndexSize(m_IndexCount);

		_newVBuffer2->SetIndexBuffer(_IB2);
		_newVBuffer2->SetIndexSize(m_IndexCount);

		m_VBufferList.insert(make_pair(name, std::move(_newVBuffer1)));
		m_VBufferList.insert(make_pair(name + L"2", std::move(_newVBuffer2)));

		vector<float> _numList1;
		vector<float> _numList2;
		_numList1.push_back(minX1);
		_numList1.push_back(minY);
		_numList1.push_back(minZ1);
		_numList1.push_back(maxX1);
		_numList1.push_back(maxY);
		_numList1.push_back(maxZ1);

		_numList2.push_back(minX2);
		_numList2.push_back(minY);
		_numList2.push_back(minZ2);
		_numList2.push_back(maxX2);
		_numList2.push_back(maxY);
		_numList2.push_back(maxZ2);

		m_minmaxList.insert(make_pair(name + L"Col", _numList1));
		m_minmaxList.insert(make_pair(name + L"2" + L"Col", _numList2));
	}

	else if (name == L"floor_l_tex")
	{
		UINT _vcount = 8;
		UINT _tcount = 26;

		float minX1 = 0;
		float minX2 = 0;
		float minY = 0;
		float minZ1 = 0;
		float minZ2 = 0;
		float maxX1 = 0;
		float maxX2 = 0;
		float maxY = 0;
		float maxZ1 = 0;
		float maxZ2 = 0;

		for (size_t i = 0; i < meshData->m_opt_vertex.size(); i++)
		{
			if (minX1 == 0)
				minX1 = meshData->m_opt_vertex[i]->m_pos.x;
			if (minY == 0)
				minY = meshData->m_opt_vertex[i]->m_pos.y;
			if (minZ1 == 0)
				minZ1 = meshData->m_opt_vertex[i]->m_pos.z;
			if (maxX2 == 0)
				maxX2 = meshData->m_opt_vertex[i]->m_pos.x;
			if (maxY == 0)
				maxY = meshData->m_opt_vertex[i]->m_pos.y;
			if (maxZ2 == 0)
				maxZ2 = meshData->m_opt_vertex[i]->m_pos.z;

			minX1 = min(minX1, meshData->m_opt_vertex[i]->m_pos.x);
			minY = min(minY, meshData->m_opt_vertex[i]->m_pos.y);
			minZ1 = min(minZ1, meshData->m_opt_vertex[i]->m_pos.z);
			maxX2 = max(maxX2, meshData->m_opt_vertex[i]->m_pos.x);
			maxY = max(maxY, meshData->m_opt_vertex[i]->m_pos.y);
			maxZ2 = max(maxZ2, meshData->m_opt_vertex[i]->m_pos.z);
		}

		minX1 += 0.2f;
		minX2 = minX1 + 7.f;
		minY += 0.2f;
		minZ1 += 0.2f;
		minZ2 = minZ1 + 6.f;
		maxX1 = minX2 - 0.1f;
		maxX2 -= 0.2f;
		maxZ1 = maxZ2 - 0.2f;
		maxZ2 -= 0.2f;

		VertexStruct::PosColor vertices1[] =
		{
			{ XMFLOAT3(maxX1, minY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX1, maxY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX1, maxY, minZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX1, minY, minZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, minY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, maxY, maxZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, maxY, minZ1), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX1, minY, minZ1), XMFLOAT4((const float*)&Colors::Green)  }
		};

		VertexStruct::PosColor vertices2[] =
		{
			{ XMFLOAT3(maxX2, minY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX2, maxY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX2, maxY, minZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX2, minY, minZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, minY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, maxY, maxZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, maxY, minZ2), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX2, minY, minZ2), XMFLOAT4((const float*)&Colors::Green)  }
		};

		GeometryBuffer* _newVBuffer1 = new GeometryBuffer();
		GeometryBuffer* _newVBuffer2 = new GeometryBuffer();
		_newVBuffer1->m_Name = name;
		_newVBuffer2->m_Name = name;

		ID3D11Buffer* _VB1;
		ID3D11Buffer* _IB1;

		ID3D11Buffer* _VB2;
		ID3D11Buffer* _IB2;

		D3D11_BUFFER_DESC vbd1;
		vbd1.Usage = D3D11_USAGE_IMMUTABLE;
		vbd1.ByteWidth = sizeof(VertexStruct::PosColor) * 8;
		vbd1.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd1.CPUAccessFlags = 0;
		vbd1.MiscFlags = 0;
		vbd1.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData1;
		vinitData1.pSysMem = vertices1;
		m_D3dDevice->CreateBuffer(&vbd1, &vinitData1, &_VB1);

		_newVBuffer1->SetVertexBuffer(_VB1);
		_newVBuffer1->SetVertexSize(_vcount);

		D3D11_BUFFER_DESC vbd2;
		vbd2.Usage = D3D11_USAGE_IMMUTABLE;
		vbd2.ByteWidth = sizeof(VertexStruct::PosColor) * 8;
		vbd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd2.CPUAccessFlags = 0;
		vbd2.MiscFlags = 0;
		vbd2.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData2;
		vinitData2.pSysMem = vertices2;
		m_D3dDevice->CreateBuffer(&vbd2, &vinitData2, &_VB2);

		_newVBuffer2->SetVertexBuffer(_VB2);
		_newVBuffer2->SetVertexSize(_vcount);

		UINT indices[] =
		{
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			1, 5,
			5, 4,
			4, 0,
			5, 6,
			6, 7,
			7, 4,
			4, 5,
			2, 6,
			7, 3
		};

		m_IndexCount = _tcount;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
		m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB1);
		m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB2);

		_newVBuffer1->SetIndexBuffer(_IB1);
		_newVBuffer1->SetIndexSize(m_IndexCount);

		_newVBuffer2->SetIndexBuffer(_IB2);
		_newVBuffer2->SetIndexSize(m_IndexCount);

		m_VBufferList.insert(make_pair(name, std::move(_newVBuffer1)));
		m_VBufferList.insert(make_pair(name + L"2", std::move(_newVBuffer2)));

		vector<float> _numList1;
		vector<float> _numList2;
		_numList1.push_back(minX1);
		_numList1.push_back(minY);
		_numList1.push_back(minZ1);
		_numList1.push_back(maxX1);
		_numList1.push_back(maxY);
		_numList1.push_back(maxZ1);

		_numList2.push_back(minX2);
		_numList2.push_back(minY);
		_numList2.push_back(minZ2);
		_numList2.push_back(maxX2);
		_numList2.push_back(maxY);
		_numList2.push_back(maxZ2);

		m_minmaxList.insert(make_pair(name + L"Col", _numList1));
		m_minmaxList.insert(make_pair(name + L"2" + L"Col", _numList2));
	}

	else
	{
		UINT _vcount = 8;
		UINT _tcount = 26;

		float minX = 0;
		float minY = 0;
		float minZ = 0;
		float maxX = 0;
		float maxY = 0;
		float maxZ = 0;

		for (size_t i = 0; i < meshData->m_opt_vertex.size(); i++)
		{
			if (minX == 0)
				minX = meshData->m_opt_vertex[i]->m_pos.x;
			if (minY == 0)
				minY = meshData->m_opt_vertex[i]->m_pos.y;
			if (minZ == 0)
				minZ = meshData->m_opt_vertex[i]->m_pos.z;
			if (maxX == 0)
				maxX = meshData->m_opt_vertex[i]->m_pos.x;
			if (maxY == 0)
				maxY = meshData->m_opt_vertex[i]->m_pos.y;
			if (maxZ == 0)
				maxZ = meshData->m_opt_vertex[i]->m_pos.z;

			minX = min(minX, meshData->m_opt_vertex[i]->m_pos.x);
			minY = min(minY, meshData->m_opt_vertex[i]->m_pos.y);
			minZ = min(minZ, meshData->m_opt_vertex[i]->m_pos.z);
			maxX = max(maxX, meshData->m_opt_vertex[i]->m_pos.x);
			maxY = max(maxY, meshData->m_opt_vertex[i]->m_pos.y);
			maxZ = max(maxZ, meshData->m_opt_vertex[i]->m_pos.z);
		}

		minX += 0.2f;
		minY += 0.2f;
		minZ += 0.2f;
		maxX -= 0.2f;
		maxY -= 0.2f;
		maxZ -= 0.2f;

		if (name == L"Center")
		{
			minX = -0.5f;
			minY = 0;
			minZ = -0.5f;
			maxX = 0.5f;
			maxY = 0.3f;
			maxZ = 0.5f;

			colX = minX - 0.2f;
			colZ = minZ - 0.2f;
			colX2 = maxX + 0.2f;
			colZ2 = maxZ + 0.2f;
		}

		if (name == L"counter")
		{
			maxX -= 1.8f;
		}

		VertexStruct::PosColor vertices[] =
		{
			{ XMFLOAT3(maxX, minY, maxZ), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX, maxY, maxZ), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX, maxY, minZ), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(maxX, minY, minZ), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX, minY, maxZ), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX, maxY, maxZ), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX, maxY, minZ), XMFLOAT4((const float*)&Colors::Green)  },
			{ XMFLOAT3(minX, minY, minZ), XMFLOAT4((const float*)&Colors::Green)  }
		};

		GeometryBuffer* _newVBuffer = new GeometryBuffer();
		_newVBuffer->m_Name = name;

		ID3D11Buffer* _VB;
		ID3D11Buffer* _IB;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(VertexStruct::PosColor) * 8;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices;
		m_D3dDevice->CreateBuffer(&vbd, &vinitData, &_VB);

		_newVBuffer->SetVertexBuffer(_VB);
		_newVBuffer->SetVertexSize(_vcount);

		UINT indices[] =
		{
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			1, 5,
			5, 4,
			4, 0,
			5, 6,
			6, 7,
			7, 4,
			4, 5,
			2, 6,
			7, 3
		};

		m_IndexCount = _tcount;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
		m_D3dDevice->CreateBuffer(&ibd, &iinitData, &_IB);

		_newVBuffer->SetIndexBuffer(_IB);
		_newVBuffer->SetIndexSize(m_IndexCount);

		m_VBufferList.insert(make_pair(name, std::move(_newVBuffer)));

		vector<float> _numList;
		_numList.push_back(minX);
		_numList.push_back(minY);
		_numList.push_back(minZ);
		_numList.push_back(maxX);
		_numList.push_back(maxY);
		_numList.push_back(maxZ);

		m_minmaxList.insert(make_pair(name + L"Col", _numList));
	}
}

