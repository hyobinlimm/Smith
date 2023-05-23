#pragma once

#include <vector>
#include "HimeRendererStructs.h"

class DXObjectBase;
class ResourceManager;
class MeshObject;

/// <summary>
/// DXObject�� �����ϰ� ���� �� �ش�.
/// ���������� ���� ���� �ʴ´�.
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
	DXObjectBase* CreateDXObject_Axis();		// �˰� ���� �ϵ��ڵ� ���� �Լ�
	DXObjectBase* CreateDXObject_Grid();		// �˰� ���� �ϵ��ڵ� ���� �Լ�
	
	
	void CreateDXObject_Axis(std::vector<DXObjectBase*>& Objects);		// �˰� ���� �ϵ��ڵ� ���� �Լ�
	void CreateDXObject_Grid(std::vector<DXObjectBase*>& Objects);		// �˰� ���� �ϵ��ڵ� ���� �Լ�

#pragma region MK3 - Ÿ�Կ� ���������� ���� ����

	void MakeHierarchy_VS3(std::vector<DXObjectBase*>& meshObject); /// meshObject���� �θ�-�ڽ� ���踦 ������ش�.
	void MakeLocalTM_VS3(std::vector<DXObjectBase*>& meshObject);
	void MakeSkinnedNodeTM_VS3(std::vector<DXObjectBase*>& meshObject);
#pragma endregion

#pragma region MK4 - MK4 �������� �̸� ASEparser�� �ε��� ���� �ִ� ���� // Ÿ�Կ� ���������� ����. 
public:
	void CreateDXObject(std::wstring IDName, std::vector<DXObjectBase*>& Objects);

private:
	void CreateMeshBuildInfo(std::wstring IDName, std::wstring nodeName);

	void MakeHierarchy_VS4(std::vector<DXObjectBase*>& meshObject);

#pragma endregion

private:
	// Factory�� ������ ������ ���ؼ� �ʿ��� Ŭ������
	ID3D11Device* m_pD3dDevice;						// D3D11 ����̽�
	ID3D11DeviceContext* m_pDeviceContext;		// ����̽� ���ؽ�Ʈ

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

