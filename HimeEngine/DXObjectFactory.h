#pragma once

#include <vector>
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
public:
	DXObjectBase* CreateDXObject_Axis();		// 죄가 많은 하드코딩 생성 함수
	DXObjectBase* CreateDXObject_Grid();		// 죄가 많은 하드코딩 생성 함수
	
	
	void CreateDXObject_Axis(std::vector<DXObjectBase*>& Objects);		// 죄가 많은 하드코딩 생성 함수
	void CreateDXObject_Grid(std::vector<DXObjectBase*>& Objects);		// 죄가 많은 하드코딩 생성 함수

#pragma region MK3 - 타입에 종속적이지 않은 버전

	void MakeHierarchy_VS3(std::vector<DXObjectBase*>& meshObject); /// meshObject간의 부모-자식 관계를 만들어준다.
	void MakeLocalTM_VS3(std::vector<DXObjectBase*>& meshObject);
	void MakeSkinnedNodeTM_VS3(std::vector<DXObjectBase*>& meshObject);
#pragma endregion

#pragma region MK4 - MK4 엔진에서 미리 ASEparser를 로드해 놓고 있는 버전 // 타입에 종속적이지 않음. 
public:
	void CreateDXObject(std::wstring IDName, std::vector<DXObjectBase*>& Objects);

private:
	void CreateMeshBuildInfo(std::wstring IDName, std::wstring nodeName);

	void MakeHierarchy_VS4(std::vector<DXObjectBase*>& meshObject);

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

