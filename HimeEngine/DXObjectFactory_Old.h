#pragma once

#include <string>
#include "DirectXDefine.h"
#include "HimeRendererStructs.h"


class DXObjectBase;
class ResourceManager;
class MeshObject;

/// <summary>
/// DXObject를 생성하고 리턴 해 준다.
/// 내부적으로 보관 하지 않는다.
/// 
/// 2022.07.15 LeHideLogic
/// </summary>
class DXObjectFactory
{
public:
	DXObjectFactory();
	virtual ~DXObjectFactory();

	///--------------------------------------------------
	/// mk1. 다형적 동작을 기대하고 부모의 클래스를 뱉는 케이스바이케이스
	/// Abstraction
public:
#pragma region MK1
	DXObjectBase* CreateDXObject_Axis();		// 죄가 많은 하드코딩 생성 함수
	DXObjectBase* CreateDXObject_Grid();		// 죄가 많은 하드코딩 생성 함수
	DXObjectBase* CreateDXObject_ASEParser(std::string name, XMVECTOR pos);	// 1개만 리턴하는 함수
#pragma endregion

#pragma region MK2
	void CreateDXObject(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);
	void CreateDXObject_Axis(std::vector<DXObjectBase*>& Objects);		// 죄가 많은 하드코딩 생성 함수
	void CreateDXObject_Grid(std::vector<DXObjectBase*>& Objects);		// 죄가 많은 하드코딩 생성 함수

	/// info가 개별적으로 들어와야지 사용가능한 버전이다. 
	void CreateDXObject_ASEParser(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);	// 1개만 리턴하는 함수
	void CreateDXObjects_ASEParser(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);
	void CreateDXObjects_ASEParserSkin(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);

	ID3D11RasterizerState* GetRenderState(eObjType ObjType);
#pragma endregion


	///--------------------------------------------------
	/// mk2. 그것을 템플릿으로 만든 버전
	/// Generic
	//template<typename T>
	//T* CreateDXObject(MeshBuildInfo* info);
	//template<typename T>
	//T* CreateDXObject(std::string infoScriptFileName);
	///--------------------------------------------------

private:
#pragma region MK1
	void MakeHierarchy(std::vector<MeshObject*>& meshObject); /// meshObject간의 부모-자식 관계를 만들어준다.
	void MakeLocalTM(std::vector<MeshObject*>& meshObject); /// meshObject의 LocalTM를 만들자.
	
	// 애니메이션에 필요한 함수
	//void MakeAnimationTM(MeshObjects* meshObject); /// 애니메이션 좌표를 받아와서 localTM에 대입하자.
	//void MakeAnimation_POS(Animation* _nowAnimtion, ASEObjects* meshObject, int& _posIndex);
	//void MakeAnimation_ROT(Animation* _nowAnimtion, ASEObjects* meshObject, int& _rotIndex);
	
	// 스키닝에 필요한 함수
	// 스키닝 오브젝트인지 검사하는 함수
	// todo:: 이거 매쉬오브젝트 안으로 넣는게 맞을 것 같아서 고민중. 
	//void IsSkinningObject(ASEParser::GeomObject* meshData, ASEObjects* meshObject);
	void MakeSkinnedNodeTM(std::vector<MeshObject*>& meshObject);
	//void MakeBoneWorldTM(std::vector<ASEObjects*>& meshObject);
#pragma endregion

#pragma region MK2
	void MakeHierarchy_VS2(std::vector<DXObjectBase*>& meshObject); /// meshObject간의 부모-자식 관계를 만들어준다.
	void MakeLocalTM_VS2(std::vector<DXObjectBase*>& meshObject); /// meshObject의 LocalTM를 만들자.

	// 애니메이션에 필요한 함수
	//void MakeAnimationTM2(MeshObjects* meshObject); /// 애니메이션 좌표를 받아와서 localTM에 대입하자.
	//void MakeAnimation_POS(Animation* _nowAnimtion, ASEObjects* meshObject, int& _posIndex);
	//void MakeAnimation_ROT(Animation* _nowAnimtion, ASEObjects* meshObject, int& _rotIndex);

	// 스키닝에 필요한 함수
	void MakeSkinnedNodeTM_VS2(std::vector<DXObjectBase*>& meshObject);
	//void MakeBoneWorldTM(std::vector<ASEObjects*>& meshObject);
#pragma endregion

#pragma region MK3 - 타입에 종속적이지 않은 버전
	/// info에 ase파일 이름이 들어왔을 경우,
	void CreateDXObjects_ASEParserAll(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);


	void MakeHierarchy_VS3(std::vector<DXObjectBase*>& meshObject); /// meshObject간의 부모-자식 관계를 만들어준다.
	void MakeLocalTM_VS3(std::vector<DXObjectBase*>& meshObject);
	void MakeSkinnedNodeTM_VS3(std::vector<DXObjectBase*>& meshObject);
#pragma endregion

#pragma region MK4 - MK4 엔진에서 미리 ASEparser를 로드해 놓고 있는 버전 // 타입에 종속적이지 않음. 
public:
	void CreateDXObject(std::string IDName, std::vector<DXObjectBase*>& Objects);

private:
	void CreateMeshBuildInfo(std::string fileName, std::string nodeName);

#pragma endregion

private:
	// Factory가 뭔가를 만들어내기 위해서 필요한 클래스들
	ID3D11Device* m_pD3dDevice;						// D3D11 디바이스
	ID3D11DeviceContext* m_pDeviceContext;		// 디바이스 컨텍스트

	ResourceManager* m_pResourceManager;

public:
	std::vector<MeshBuildInfo*> m_MeshInfoList;
	
};

//template<typename T>
//T* DXObjectFactory::CreateDXObject(MeshBuildInfo* info)
//{
//	return T();
//}
//
//template<typename T>
//T* DXObjectFactory::CreateDXObject(std::string infoScriptFileName)
//{
//	return nullptr;
//}

