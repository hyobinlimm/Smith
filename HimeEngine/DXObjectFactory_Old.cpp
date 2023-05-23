
#include "HimeDevice.h"
#include "DirectXDefine.h"

#include "ResourceManager.h"
#include "GeometryBuffer.h"

// 팩토리는 모든 오브젝트를 알아야 한다.
#include "HelperObjects.h"
#include "MeshObject.h"
#include "SkinningObject.h"

#include "DXObjectFactory.h"
#include "NodeInfo.h"

DXObjectFactory::DXObjectFactory()
{
	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();

	m_pResourceManager = ResourceManager::GetInst();
}

DXObjectFactory::~DXObjectFactory()
{

}

#pragma region MK1 오브젝트 타입마다 바깥에서 Create 함수를 호출하는 방식

DXObjectBase* DXObjectFactory::CreateDXObject_Axis()
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetWireframeRS();

	// DX오브젝트 자체를 만들어준다.
	/// 특정한 한 내용이 아니라 일반화 된 오브젝트들이어야만 한다.
	/// 예를 들어, Axis나 Grid가 아니라 HelperObject이고
	/// 겐지나 돼지가 아니라 DXMeshObject또는 DXSkinningMeshObject이어야 한다.
	HelperObjects* _newAsix = new HelperObjects(_RS);

	// 리소스를 연결 시켜준다.
	GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer("Axis");
	_newAsix->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
	_newAsix->SetIndexSize(_nowGBuffer->GetIndexSize());
	return _newAsix;
}

DXObjectBase* DXObjectFactory::CreateDXObject_Grid()
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetWireframeRS();

	HelperObjects* _newGrid = new HelperObjects(_RS);

	// 리소스를 연결 시켜준다.
	GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer("Grid");
	_newGrid->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
	_newGrid->SetIndexSize(_nowGBuffer->GetIndexSize());

	return _newGrid;
}

/// ASE 파일을 읽어서, N개의 DXObject가 나오지만 0번째 DXObject만 리턴한다.
DXObjectBase* DXObjectFactory::CreateDXObject_ASEParser(std::string name, XMVECTOR pos)
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetSolidRS();

	// 이름을 가져와서 리소스 매니저에게 ASE 파일을 가져와 달라고 부탁.
	m_pResourceManager->LoadASEObject(name);

	MeshObject* _newMesh = new MeshObject(_RS, pos);

	// 텍스처를 연결해준다. 
	_newMesh->SetTexResource(name);

	// game엔진 불러온 애를 찾아서 리소스를 연결
	GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(name);
	_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
	_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

	return _newMesh;
}

#pragma endregion

#pragma region MK2 오브젝트 타입마다 바깥에서 CreateDXObject 함수만 호출하는 방식(meshInfo로 내가 어떤걸 생성할지 결정)
void DXObjectFactory::CreateDXObject(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects)
{
	switch (pmeshInfo->ObjType)
	{
	case eObjType::Helper:
		{
			if (pmeshInfo->nodeName == "Axis")
			{
				CreateDXObject_Axis(Objects);
			}
			else if (pmeshInfo->nodeName == "Grid")
			{
				CreateDXObject_Grid(Objects);
			}
		}
		break;

	case eObjType::StaticMesh:
		CreateDXObject_ASEParser(pmeshInfo, Objects);
		break;

	case eObjType::Animation:
		CreateDXObjects_ASEParser(pmeshInfo, Objects);
		break;

	case eObjType::Skinning:
		CreateDXObjects_ASEParserSkin(pmeshInfo, Objects);
		break;

	case eObjType::Bone:

	default:
		break;
	}
}

void DXObjectFactory::CreateDXObject_Axis(std::vector<DXObjectBase*>& Objects)
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetWireframeRS();

	// DX오브젝트 자체를 만들어준다.
	/// 특정한 한 내용이 아니라 일반화 된 오브젝트들이어야만 한다.
	/// 예를 들어, Axis나 Grid가 아니라 HelperObject이고
	/// 겐지나 돼지가 아니라 DXMeshObject또는 DXSkinningMeshObject이어야 한다.
	HelperObjects* _newAsix = new HelperObjects(_RS);

	// 노드이름없어서 만든 임시코드
	_newAsix->m_NodeName = "Axis";

	// 리소스를 연결 시켜준다.
	GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer("Axis");
	_newAsix->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
	_newAsix->SetIndexSize(_nowGBuffer->GetIndexSize());

	Objects.emplace_back(_newAsix);

	CreateMeshBuildInfo(_newAsix->m_NodeName, _newAsix->m_NodeName);
}

void DXObjectFactory::CreateDXObject_Grid(std::vector<DXObjectBase*>& Objects)
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetWireframeRS();

	HelperObjects* _newGrid = new HelperObjects(_RS);

	// 노드이름없어서 만든 임시코드
	_newGrid->m_NodeName = "Grid";

	// 리소스를 연결 시켜준다.
	GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer("Grid");
	_newGrid->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
	_newGrid->SetIndexSize(_nowGBuffer->GetIndexSize());

	Objects.emplace_back(_newGrid);

	CreateMeshBuildInfo(_newGrid->m_NodeName, _newGrid->m_NodeName);
}

void DXObjectFactory::CreateDXObject_ASEParser(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects)
{
	ID3D11RasterizerState* _RS = GetRenderState(pmeshInfo->ObjType);

	// 이름을 가져와서 리소스 매니저에게 ASE 파일을 가져와 달라고 부탁.
	m_pResourceManager->LoadASEObject(pmeshInfo->nodeName);

	for (auto _nowKVP : m_pResourceManager->m_NodeInfoList)
	{
		std::string _nodeName = _nowKVP.first;

		MeshObject* _newMesh = new MeshObject(_RS, pmeshInfo->Pos);

		// 노드이름없어서 만든 임시코드
		_newMesh->m_NodeName = pmeshInfo->nodeName;

		// 텍스처를 연결해준다. 
		_newMesh->SetTexResource(pmeshInfo->nodeName);

		// game엔진 불러온 애를 찾아서 리소스를 연결
		GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(_nodeName);
		_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
		_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

		Objects.emplace_back(std::move(_newMesh));
	}
}

void DXObjectFactory::CreateDXObjects_ASEParser(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects)
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetSolidRS();

	// 이름을 가져와서 리소스 매니저에게 ASE 파일을 가져와 달라고 부탁.
	m_pResourceManager->LoadASEObject(pmeshInfo->nodeName);

	vector<MeshObject*> _meshObjects;

	for (auto _nowKVP : m_pResourceManager->m_NodeInfoList)
	{
		std::string _nodeName = _nowKVP.first;

		MeshObject* _newMesh = new MeshObject(_RS, pmeshInfo->Pos);

		_newMesh->m_NodeName = pmeshInfo->nodeName;
		// 텍스처를 연결해준다. 
		_newMesh->SetTexResource(pmeshInfo->nodeName);

		// game엔진 불러온 애를 찾아서 리소스를 연결
		GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(_nodeName);
		_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
		_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

		_meshObjects.emplace_back(std::move(_newMesh));

	}

	// GeomObject의 부모 - 자식 관계를 만들어주자.
	MakeHierarchy(_meshObjects);

	// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
	MakeLocalTM(_meshObjects);

	for (auto _object : _meshObjects)
	{
		Objects.emplace_back(std::move(_object));
	}
}

void DXObjectFactory::CreateDXObjects_ASEParserSkin(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects)
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetSolidRS();

	// 이름을 가져와서 리소스 매니저에게 ASE 파일을 가져와 달라고 부탁.
	m_pResourceManager->LoadASEObject(pmeshInfo->nodeName);

	vector<DXObjectBase*> _geometryObjects;

	for (auto _nowKVP : m_pResourceManager->m_NodeInfoList)
	{
		std::string _nodeName = _nowKVP.first;
		NodeInfo* _nodeInfo = _nowKVP.second;

		switch (_nodeInfo->m_type)
		{
		case eGeomobject:
			{
				if (_nodeInfo->m_IsSkinningObj == false)
				{
					MeshObject* _newMesh = new MeshObject(_RS, pmeshInfo->Pos);

					// ASEParser에서 받아온 내이름, 부모이름, nodeTM, 스키닝 오브젝트인지을 넣어준다. 
					_newMesh->m_NodeName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_NodeName;
					_newMesh->m_ParentName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_ParentName;
					_newMesh->m_IsSkinningObj = m_pResourceManager->m_NodeInfoList[_nodeName]->m_IsSkinningObj;
					_newMesh->m_pAnimation = m_pResourceManager->m_NodeInfoList[_nodeName]->m_pAnimation;

					// 텍스처를 연결해준다. 
					_newMesh->SetTexResource(pmeshInfo->nodeName);

					// game엔진 불러온 애를 찾아서 리소스를 연결
					GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(_nodeName);
					_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
					_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

					_geometryObjects.push_back(std::move(_newMesh));
				}
				else
				{
					SkinningObject* _newMesh = new SkinningObject(_RS, pmeshInfo->Pos);

					// ASEParser에서 받아온 내이름, 부모이름, nodeTM, 스키닝 오브젝트인지을 넣어준다. 
					_newMesh->m_NodeName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_NodeName;
					_newMesh->m_ParentName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_ParentName;
					_newMesh->m_IsSkinningObj = m_pResourceManager->m_NodeInfoList[_nodeName]->m_IsSkinningObj;
					_newMesh->m_BoneList = m_pResourceManager->m_NodeInfoList[_nodeName]->m_BoneList;
					_newMesh->m_pAnimation = m_pResourceManager->m_NodeInfoList[_nodeName]->m_pAnimation;

					// 텍스처를 연결해준다. 
					_newMesh->SetTexResource(pmeshInfo->nodeName);

					// game엔진 불러온 애를 찾아서 리소스를 연결
					GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(_nodeName);
					_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
					_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

					_geometryObjects.push_back(std::move(_newMesh));
				}

			}
			break;

		default:
			break;
		}
	}

	// GeomObject의 부모 - 자식 관계를 만들어주자.
	MakeHierarchy_VS3(_geometryObjects);

	// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
	MakeLocalTM_VS3(_geometryObjects);

	// 오브젝트 들에서 본의 nodeTM과 worldTM를 받아서 넣어주자. 
	MakeSkinnedNodeTM_VS3(_geometryObjects);

	for (auto _object : _geometryObjects)
	{
		Objects.push_back(std::move(_object));
	}
}

#pragma endregion

ID3D11RasterizerState* DXObjectFactory::GetRenderState(eObjType ObjType)
{
	switch (ObjType)
	{
	case eObjType::Helper:
		return HimeDevice::GetInst()->GetWireframeRS();
	case eObjType::StaticMesh:
	case eObjType::Animation:
	case eObjType::Skinning:
	case eObjType::Bone:
		return HimeDevice::GetInst()->GetSolidRS();
	default:
		break;
	}

	return nullptr;
}

#pragma  region MK1 NodeTM를 지정해주는 함수들
void DXObjectFactory::MakeHierarchy(std::vector<MeshObject*>& meshObject)
{
	/// 벡터에 들어있는 GeomObject들의 부모-자식 관계를 만들자.

	bool _isParentFounded = false;

	//	1. for each를 사용해 이중포문을 돌면서,
	//		나의 m_ParentNodeName과 같은 targetmesh의 mNodeName 있는지 찾는다.
	for (size_t i = 0; i < meshObject.size(); i++)
	{
		_isParentFounded = false;

		for (size_t j = 0; j < meshObject.size(); j++)
		{
			//	1-1. 나의 mNodeName과 targetmesh의 mNodeName 같다면 무시한다. 
			if (meshObject[i]->m_NodeName == meshObject[j]->m_NodeName)
			{
				meshObject[i]->m_pParentNode = nullptr;
				continue;
			}
			//	2. 찾았다면 나의  m_ParentNode 에 GeomObject의 주소를 넣어준다. 
			if (meshObject[i]->m_ParentName == meshObject[j]->m_NodeName)
			{
				meshObject[i]->m_pParentNode = meshObject[j];

				_isParentFounded = true;
				break;
			}
		}

		/// 부모의 이름이 있는데, 위에서 결국 부모를 못찾은 경우
		// 부모의 이름이 있는데
		if (meshObject[i]->m_ParentName.size() != 0)
		{
			// 이러면 뭐가 문제가 있어서 결국 부모를 못찾은 것이다.
			if (_isParentFounded == false)
			{
				int kk = 0;
			}
		}
	}
}

void DXObjectFactory::MakeLocalTM(std::vector<MeshObject*>& meshObject)
{
	/// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
	/// (나의 NodeTM과 부모의 NodeTM의 역행렬을 이용해서)

	for (size_t i = 0; i < meshObject.size(); i++)
	{
		// 부모가 없으면 나의 nodeTM를 localTM에 넣어주자. 
		if (meshObject[i]->m_pParentNode == nullptr)
		{
			meshObject[i]->m_NodeData.m_LocalTM = meshObject[i]->m_NodeData.m_NodeTM;

			// decompose LocalTM 
			meshObject[i]->DecomposeLocalTM();

			continue;
		}

		// _nowMesh의 부모 NodeTM의 역행렬을 얻어오자. 
		XMMATRIX _ParentNodeTM = ::XMLoadFloat4x4(&meshObject[i]->m_pParentNode->m_NodeData.m_NodeTM);
		XMMATRIX _parentInverseNodeTM = XMMatrixInverse(nullptr, _ParentNodeTM);

		// _nowMesh->LocalTM 를 구하자. 
		// LocalTM = NodeTM * Parent’sNodeTM −1
		meshObject[i]->m_NodeData.m_LocalTM = meshObject[i]->m_NodeData.m_NodeTM * _parentInverseNodeTM;

		//meshObject[i]->mNodeTM = meshObject[i]->mLocalTM * _ParentNodeTM;

		// decompose LocalTM 
		meshObject[i]->DecomposeLocalTM();
	}
}

void DXObjectFactory::MakeSkinnedNodeTM(std::vector<MeshObject*>& meshObject)
{
	std::vector<ASEParser::Bone*> _Bones;

	// skin오브젝트의 본 리스트를 저장
	for (MeshObject* _object : meshObject)
	{
		if (_object->m_IsSkinningObj == true)
		{
			_Bones = _object->m_BoneList;

			break;
		}
	}

	// 본 리스트의 이름과 오브젝트의 이름을 찾아서 nodeTM를 넣어주자. 
	for (Bone* _bone : _Bones)
	{
		for (MeshObject* _object : meshObject)
		{
			if (_bone->m_bone_name == _object->m_NodeName)
			{
				// 첫 nodeTM를 넣어줌. 
				Matrix* _temp1 = new Matrix;
				*_temp1 = _object->m_NodeData.m_NodeTM;
				_bone->m_boneTM_NodeTM = _temp1;
				_bone->m_boneTM_NodeTM = reinterpret_cast<Matrix*>(&_object->m_NodeData.m_NodeTM);
			}
			break; // 찾았다면 안쪽 for문을 탈출하고, 바깥 쪽 for문을 돌게 하자. 
		}
	}
}

#pragma  endregion 

#pragma region MK2 NodeTM를 지정해주는 함수들

void DXObjectFactory::MakeHierarchy_VS2(std::vector<DXObjectBase*>& meshObject)
{
	/// 벡터에 들어있는 GeomObject들의 부모-자식 관계를 만들자.

	bool _isParentFounded = false;

	//	1. for each를 사용해 이중포문을 돌면서,
	//		나의 m_ParentNodeName과 같은 targetmesh의 mNodeName 있는지 찾는다.
	for (size_t i = 0; i < meshObject.size(); i++)
	{
		auto _i_skinningObj = dynamic_cast<SkinningObject*>(meshObject[i]);
		auto _i_meshObj = dynamic_cast<MeshObject*>(meshObject[i]);

		_isParentFounded = false;

		// 1. DX오브젝트가 스키닝 오브젝트인 경우
		if (_i_skinningObj != nullptr)
		{
			for (size_t j = 0; j < meshObject.size(); j++)
			{
				auto _j_skinningObj = dynamic_cast<SkinningObject*>(meshObject[j]);

				if (_j_skinningObj != nullptr)
				{
					//	1-1. 나의 mNodeName과 targetmesh의 mNodeName 같다면 무시한다. 
					if (_i_skinningObj->m_NodeName == _j_skinningObj->m_NodeName)
					{
						_i_skinningObj->m_pParentNode = nullptr;
						continue;
					}
					//	2. 찾았다면 나의  m_ParentNode 에 GeomObject의 주소를 넣어준다. 
					if (_i_skinningObj->m_ParentName == _j_skinningObj->m_NodeName)
					{
						_i_skinningObj->m_pParentNode = _j_skinningObj;

						_isParentFounded = true;
						break;
					}
				}
			}


			/// 부모의 이름이 있는데, 위에서 결국 부모를 못찾은 경우
			// 부모의 이름이 있는데
			if (_i_skinningObj->m_ParentName.size() != 0)
			{
				// 이러면 뭐가 문제가 있어서 결국 부모를 못찾은 것이다.
				if (_isParentFounded == false)
				{
					int kk = 0;
				}
			}
		}

		// 2. DX오브젝트가 메쉬 오브젝트인 경우
		if (_i_meshObj != nullptr)
		{
			for (size_t j = 0; j < meshObject.size(); j++)
			{
				auto _j_meshObj = dynamic_cast<MeshObject*>(meshObject[j]);

				if (_j_meshObj != nullptr)
				{

					//	1-1. 나의 mNodeName과 targetmesh의 mNodeName 같다면 무시한다. 
					if (_i_meshObj->m_NodeName == _j_meshObj->m_NodeName)
					{
						_i_meshObj->m_pParentNode = nullptr;
						continue;
					}
					//	2. 찾았다면 나의  m_ParentNode 에 GeomObject의 주소를 넣어준다. 
					if (_i_meshObj->m_ParentName == _j_meshObj->m_NodeName)
					{
						_i_meshObj->m_pParentNode = _j_meshObj;

						_isParentFounded = true;
						break;
					}
				}
			}
		}

	}
}

void DXObjectFactory::MakeLocalTM_VS2(std::vector<DXObjectBase*>& meshObject)
{
	/// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
/// (나의 NodeTM과 부모의 NodeTM의 역행렬을 이용해서)

	for (size_t i = 0; i < meshObject.size(); i++)
	{
		auto _i_meshObj = dynamic_cast<SkinningObject*>(meshObject[i]);

		// 부모가 없으면 나의 nodeTM를 localTM에 넣어주자. 
		if (_i_meshObj->m_pParentNode == nullptr)
		{
			_i_meshObj->m_NodeData.m_NodeTM = _i_meshObj->m_NodeData.m_NodeTM;

			// decompose LocalTM 
			_i_meshObj->DecomposeLocalTM();

			continue;
		}

		// _nowMesh의 부모 NodeTM의 역행렬을 얻어오자. 
		XMMATRIX _ParentNodeTM = ::XMLoadFloat4x4(&_i_meshObj->m_pParentNode->m_NodeData.m_NodeTM);
		XMMATRIX _parentInverseNodeTM = XMMatrixInverse(nullptr, _ParentNodeTM);

		// _nowMesh->LocalTM 를 구하자. 
		// LocalTM = NodeTM * Parent’sNodeTM −1
		_i_meshObj->m_NodeData.m_NodeTM = _i_meshObj->m_NodeData.m_NodeTM * _parentInverseNodeTM;

		//meshObject[i]->mNodeTM = meshObject[i]->mLocalTM * _ParentNodeTM;

		// decompose LocalTM 
		_i_meshObj->DecomposeLocalTM();
	}
}

void DXObjectFactory::MakeSkinnedNodeTM_VS2(std::vector<DXObjectBase*>& meshObject)
{
	std::vector<ASEParser::Bone*> _Bones;

	// skin오브젝트의 본 리스트를 저장
	for (auto _object : meshObject)
	{
		auto _castObj = dynamic_cast<SkinningObject*>(_object);

		if (_castObj->m_IsSkinningObj == true)
		{
			_Bones = _castObj->m_BoneList;

			break;
		}
	}

	// 본 리스트의 이름과 오브젝트의 이름을 찾아서 nodeTM를 넣어주자. 
	for (Bone* _bone : _Bones)
	{
		for (auto _object : meshObject)
		{
			auto _castObj = dynamic_cast<SkinningObject*>(_object);

			if (_bone->m_bone_name == _castObj->m_NodeName)
			{
				// 첫 nodeTM를 넣어줌. 
				Matrix* _temp1 = new Matrix;
				*_temp1 = _castObj->m_NodeData.m_NodeTM;
				_bone->m_boneTM_NodeTM = _temp1;
				_bone->m_boneTM_NodeTM = reinterpret_cast<Matrix*>(&_castObj->m_NodeData.m_NodeTM);
			}
			break; // 찾았다면 안쪽 for문을 탈출하고, 바깥 쪽 for문을 돌게 하자. 
		}
	}
}

#pragma endregion

#pragma region MK3 NodeTM를 지정해주는 함수들 - 상속계층도의 맨 위쪽으로 부모-자식 기능을 넣은 경우

void DXObjectFactory::CreateDXObjects_ASEParserAll(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects)
{
	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetSolidRS();

	// 이름을 가져와서 리소스 매니저에게 ASE 파일을 가져와 달라고 부탁.
	m_pResourceManager->LoadASEObject(pmeshInfo->nodeName);

	vector<DXObjectBase*> _geometryObjects;

	for (auto _nowKVP : m_pResourceManager->m_NodeInfoList)
	{
		std::string _nodeName = _nowKVP.first;
		NodeInfo* _nodeInfo = _nowKVP.second;

		switch (_nodeInfo->m_type)
		{
		case eGeomobject:
			{
				if (_nodeInfo->m_IsSkinningObj == false)
				{
					MeshObject* _newMesh = new MeshObject(_RS, pmeshInfo->Pos);

					// ASEParser에서 받아온 내이름, 부모이름, nodeTM, 스키닝 오브젝트인지을 넣어준다. 
					_newMesh->m_NodeName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_NodeName;
					_newMesh->m_ParentName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_ParentName;
					_newMesh->m_IsSkinningObj = m_pResourceManager->m_NodeInfoList[_nodeName]->m_IsSkinningObj;
					_newMesh->m_pAnimation = m_pResourceManager->m_NodeInfoList[_nodeName]->m_pAnimation;

					// 텍스처를 연결해준다. 
					_newMesh->SetTexResource(pmeshInfo->nodeName);

					// game엔진 불러온 애를 찾아서 리소스를 연결
					GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(pmeshInfo->nodeName);
					_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
					_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

					_geometryObjects.push_back(std::move(_newMesh));
				}
				else
				{
					SkinningObject* _newMesh = new SkinningObject(_RS, pmeshInfo->Pos);

					// ASEParser에서 받아온 내이름, 부모이름, nodeTM, 스키닝 오브젝트인지을 넣어준다. 
					_newMesh->m_NodeName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_NodeName;
					_newMesh->m_ParentName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_ParentName;
					_newMesh->m_IsSkinningObj = m_pResourceManager->m_NodeInfoList[_nodeName]->m_IsSkinningObj;
					_newMesh->m_BoneList = m_pResourceManager->m_NodeInfoList[_nodeName]->m_BoneList;
					_newMesh->m_pAnimation = m_pResourceManager->m_NodeInfoList[_nodeName]->m_pAnimation;

					// 텍스처를 연결해준다. 
					_newMesh->SetTexResource(pmeshInfo->nodeName);

					// game엔진 불러온 애를 찾아서 리소스를 연결
					GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(pmeshInfo->nodeName);
					_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
					_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

					_geometryObjects.push_back(std::move(_newMesh));
				}

			}
			break;

		default:
			break;
		}
	}

	// m_IKJoe가 들고있는 GeomObject의 부모 - 자식 관계를 만들어주자.
	MakeHierarchy_VS3(_geometryObjects);

	// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
	MakeLocalTM_VS3(_geometryObjects);

	// 오브젝트 들에서 본의 nodeTM과 worldTM를 받아서 넣어주자. 
	MakeSkinnedNodeTM_VS3(_geometryObjects);

	for (auto _object : _geometryObjects)
	{
		Objects.push_back(std::move(_object));
	}
}

void DXObjectFactory::CreateMeshBuildInfo(std::string fileName, std::string nodeName)
{
	/// 게임 엔진에서 object들의 이름을 넘기기 위한 구조체 

	MeshBuildInfo* _newMeshInfo = new MeshBuildInfo();

	_newMeshInfo->fileName = fileName;
	_newMeshInfo->nodeName = nodeName;
	m_MeshInfoList.emplace_back(_newMeshInfo);

	_newMeshInfo = nullptr;
}

void DXObjectFactory::MakeHierarchy_VS3(std::vector<DXObjectBase*>& meshObject)
{
	/// 벡터에 들어있는 GeomObject들의 부모-자식 관계를 만들자.

	bool _isParentFounded = false;

	// 1. for each를 사용해 이중포문을 돌면서,
	// 나의 m_ParentNodeName과 같은 targetmesh의 mNodeName 있는지 찾는다.
	for (size_t i = 0; i < meshObject.size(); i++)
	{
		auto& _nowNodeInfo = meshObject[i];

		_isParentFounded = false;

		for (size_t j = 0; j < meshObject.size(); j++)
		{
			auto& _targetNodeInfo = meshObject[j];

			//	1-1. 나의 mNodeName과 targetmesh의 mNodeName 같다면 무시한다. 
			if (_nowNodeInfo->m_NodeName == _targetNodeInfo->m_NodeName)
			{
				_nowNodeInfo->m_pParentNodeInfo = nullptr;
				continue;
			}

			//	2. 찾았다면 나의  m_ParentNode 에 GeomObject의 주소를 넣어준다. 
			if (_nowNodeInfo->m_ParentName == _targetNodeInfo->m_NodeName)
			{
				_nowNodeInfo->m_pParentNodeInfo = _targetNodeInfo;

				_isParentFounded = true;
				break;
			}
		}

		/// 부모의 이름이 있는데, 위에서 결국 부모를 못찾은 경우
		// 부모의 이름이 있는데
		if (_nowNodeInfo->m_ParentName.size() != 0)
		{
			// 이러면 뭐가 문제가 있어서 결국 부모를 못찾은 것이다.
			if (_isParentFounded == false)
			{
				int kk = 0;
			}
		}
	}
}

void DXObjectFactory::MakeLocalTM_VS3(std::vector<DXObjectBase*>& meshObject)
{
	/// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
	/// (나의 NodeTM과 부모의 NodeTM의 역행렬을 이용해서)

	for (size_t i = 0; i < meshObject.size(); i++)
	{
		auto _NowMeshObj = meshObject[i];

		// 부모가 없으면 나의 nodeTM를 localTM에 넣어주자. 
		if (_NowMeshObj->m_pParentNodeInfo == nullptr)
		{
			_NowMeshObj->m_NodeData.m_LocalTM = _NowMeshObj->m_NodeData.m_NodeTM;

			// decompose LocalTM 
			_NowMeshObj->DecomposeLocalTM();

			continue;
		}

		// _nowMesh의 부모 NodeTM의 역행렬을 얻어오자. 
		XMMATRIX _ParentNodeTM = ::XMLoadFloat4x4(&_NowMeshObj->m_pParentNodeInfo->m_NodeData.m_NodeTM);
		XMMATRIX _parentInverseNodeTM = XMMatrixInverse(nullptr, _ParentNodeTM);

		// _nowMesh->LocalTM 를 구하자. 
		// LocalTM = NodeTM * Parent’sNodeTM −1
		_NowMeshObj->m_NodeData.m_LocalTM = _NowMeshObj->m_NodeData.m_NodeTM * _parentInverseNodeTM;

		// 내 노드TM(WorldTM)은 내 부모의 노드TM(WorldTM)에 내 로컬TM을 곱한 것과 같다.
		meshObject[i]->m_NodeData.m_NodeTM = meshObject[i]->m_NodeData.m_LocalTM * _ParentNodeTM;

		// decompose LocalTM 
		_NowMeshObj->DecomposeLocalTM();
	}
}

void DXObjectFactory::MakeSkinnedNodeTM_VS3(std::vector<DXObjectBase*>& meshObject)
{
	std::vector<ASEParser::Bone*> _Bones;

	// skin오브젝트의 본 리스트를 저장
	for (auto _nowObject : meshObject)
	{
		if (_nowObject->m_IsSkinningObj == true)
		{
			_Bones = _nowObject->m_BoneList;
		}
		break;
	}

	// 본 리스트의 이름과 오브젝트의 이름을 찾아서 nodeTM를 넣어주자. 
	for (Bone* _bone : _Bones)
	{
		for (auto _nowObject : meshObject)
		{
			if (_bone->m_bone_name == _nowObject->m_NodeName)
			{
				// 첫 nodeTM를 넣어줌. 
				Matrix* _temp1 = new Matrix;
				*_temp1 = _nowObject->m_NodeData.m_NodeTM;
				_bone->m_boneTM_NodeTM = _temp1;
				_bone->m_boneTM_NodeTM = reinterpret_cast<Matrix*>(&_nowObject->m_NodeData.m_NodeTM);
		
				MeshObject* _nowMeshObject = dynamic_cast<MeshObject*>(_nowObject);

				if (_nowMeshObject != nullptr)
				{
					///XMMATRIX _worldTM = XMLoadFloat4x4(&_nowMeshObject->GetWorld());
					_bone->m_boneTM_WorldTM = reinterpret_cast<Matrix*>(&_nowMeshObject->GetWorld());
				}

				break; // 찾았다면 안쪽 for문을 탈출하고, 바깥 쪽 for문을 돌게 하자. 
			}
		}
	}
}

#pragma endregion

#pragma region MK4 엔진에서 미리 ASEparser를 로드해 놓고 있는 버전
void DXObjectFactory::CreateDXObject(std::string IDName, std::vector<DXObjectBase*>& Objects)
{
	/// TODO : - 타입을 모르는데 어떻게 오브젝트를 만들 수 있을까? 

	ID3D11RasterizerState* _RS = HimeDevice::GetInst()->GetSolidRS();

	if (IDName == "Axis")
	{
		CreateDXObject_Axis(Objects);
	}
	else if (IDName == "Grid")
	{
		CreateDXObject_Grid(Objects);
	}
	else
	{
		// 이름을 가져와서 리소스 매니저에게 ASE 파일을 가져와 달라고 부탁.
		m_pResourceManager->LoadASEObject_BuildInfo(IDName);

		vector<DXObjectBase*> _geometryObjects;

		for (auto _nowKVP : m_pResourceManager->m_NodeInfoList)
		{
			std::string _nodeName = _nowKVP.first;
			NodeInfo* _nodeInfo = _nowKVP.second;

			switch (_nodeInfo->m_type)
			{
			case eGeomobject:
				{
					if (_nodeInfo->m_IsSkinningObj == false)
					{
						MeshObject* _newMesh = new MeshObject(_RS);

						// ASEParser에서 받아온 내이름, 부모이름, nodeTM, 스키닝 오브젝트인지을 넣어준다. 
						_newMesh->m_NodeName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_NodeName;
						_newMesh->m_ParentName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_ParentName;
						_newMesh->m_IsSkinningObj = m_pResourceManager->m_NodeInfoList[_nodeName]->m_IsSkinningObj;
						_newMesh->m_pAnimation = m_pResourceManager->m_NodeInfoList[_nodeName]->m_pAnimation;

						// 텍스처를 연결해준다. 
						_newMesh->SetTexResource(IDName);

						// game엔진 불러온 애를 찾아서 리소스를 연결
						GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(_nodeName);
						_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
						_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

						_geometryObjects.push_back(std::move(_newMesh));
						
						CreateMeshBuildInfo(IDName, _nodeName);
					}
					else
					{
						SkinningObject* _newMesh = new SkinningObject(_RS);

						// ASEParser에서 받아온 내이름, 부모이름, nodeTM, 스키닝 오브젝트인지을 넣어준다. 
						_newMesh->m_NodeName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_NodeName;
						_newMesh->m_ParentName = m_pResourceManager->m_NodeInfoList[_nodeName]->m_ParentName;
						_newMesh->m_IsSkinningObj = m_pResourceManager->m_NodeInfoList[_nodeName]->m_IsSkinningObj;
						_newMesh->m_BoneList = m_pResourceManager->m_NodeInfoList[_nodeName]->m_BoneList;
						_newMesh->m_pAnimation = m_pResourceManager->m_NodeInfoList[_nodeName]->m_pAnimation;

						// 텍스처를 연결해준다. 
						_newMesh->SetTexResource(IDName);

						// game엔진 불러온 애를 찾아서 리소스를 연결
						GeometryBuffer* _nowGBuffer = m_pResourceManager->GetGBuffer(_nodeName);
						_newMesh->SetGeometryBuffer(_nowGBuffer->GetVertexBuffer(), _nowGBuffer->GetIndexBuffer());
						_newMesh->SetIndexSize(_nowGBuffer->GetIndexSize());

						_geometryObjects.push_back(std::move(_newMesh));
						
						CreateMeshBuildInfo(IDName, _nodeName);
					}
				}
				break;

			default:
				break;
			}
		}

		// m_IKJoe가 들고있는 GeomObject의 부모 - 자식 관계를 만들어주자.
		MakeHierarchy_VS3(_geometryObjects);

		// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
		MakeLocalTM_VS3(_geometryObjects);

		// 오브젝트 들에서 본의 nodeTM과 worldTM를 받아서 넣어주자. 
		MakeSkinnedNodeTM_VS3(_geometryObjects);

		for (auto _object : _geometryObjects)
		{
			Objects.push_back(std::move(_object));
		}
	}
}

#pragma endregion