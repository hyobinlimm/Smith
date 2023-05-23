#pragma once

#include <string>
#include "DirectXDefine.h"
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
	/// mk1. ������ ������ ����ϰ� �θ��� Ŭ������ ��� ���̽��������̽�
	/// Abstraction
public:
#pragma region MK1
	DXObjectBase* CreateDXObject_Axis();		// �˰� ���� �ϵ��ڵ� ���� �Լ�
	DXObjectBase* CreateDXObject_Grid();		// �˰� ���� �ϵ��ڵ� ���� �Լ�
	DXObjectBase* CreateDXObject_ASEParser(std::string name, XMVECTOR pos);	// 1���� �����ϴ� �Լ�
#pragma endregion

#pragma region MK2
	void CreateDXObject(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);
	void CreateDXObject_Axis(std::vector<DXObjectBase*>& Objects);		// �˰� ���� �ϵ��ڵ� ���� �Լ�
	void CreateDXObject_Grid(std::vector<DXObjectBase*>& Objects);		// �˰� ���� �ϵ��ڵ� ���� �Լ�

	/// info�� ���������� ���;��� ��밡���� �����̴�. 
	void CreateDXObject_ASEParser(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);	// 1���� �����ϴ� �Լ�
	void CreateDXObjects_ASEParser(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);
	void CreateDXObjects_ASEParserSkin(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);

	ID3D11RasterizerState* GetRenderState(eObjType ObjType);
#pragma endregion


	///--------------------------------------------------
	/// mk2. �װ��� ���ø����� ���� ����
	/// Generic
	//template<typename T>
	//T* CreateDXObject(MeshBuildInfo* info);
	//template<typename T>
	//T* CreateDXObject(std::string infoScriptFileName);
	///--------------------------------------------------

private:
#pragma region MK1
	void MakeHierarchy(std::vector<MeshObject*>& meshObject); /// meshObject���� �θ�-�ڽ� ���踦 ������ش�.
	void MakeLocalTM(std::vector<MeshObject*>& meshObject); /// meshObject�� LocalTM�� ������.
	
	// �ִϸ��̼ǿ� �ʿ��� �Լ�
	//void MakeAnimationTM(MeshObjects* meshObject); /// �ִϸ��̼� ��ǥ�� �޾ƿͼ� localTM�� ��������.
	//void MakeAnimation_POS(Animation* _nowAnimtion, ASEObjects* meshObject, int& _posIndex);
	//void MakeAnimation_ROT(Animation* _nowAnimtion, ASEObjects* meshObject, int& _rotIndex);
	
	// ��Ű�׿� �ʿ��� �Լ�
	// ��Ű�� ������Ʈ���� �˻��ϴ� �Լ�
	// todo:: �̰� �Ž�������Ʈ ������ �ִ°� ���� �� ���Ƽ� �����. 
	//void IsSkinningObject(ASEParser::GeomObject* meshData, ASEObjects* meshObject);
	void MakeSkinnedNodeTM(std::vector<MeshObject*>& meshObject);
	//void MakeBoneWorldTM(std::vector<ASEObjects*>& meshObject);
#pragma endregion

#pragma region MK2
	void MakeHierarchy_VS2(std::vector<DXObjectBase*>& meshObject); /// meshObject���� �θ�-�ڽ� ���踦 ������ش�.
	void MakeLocalTM_VS2(std::vector<DXObjectBase*>& meshObject); /// meshObject�� LocalTM�� ������.

	// �ִϸ��̼ǿ� �ʿ��� �Լ�
	//void MakeAnimationTM2(MeshObjects* meshObject); /// �ִϸ��̼� ��ǥ�� �޾ƿͼ� localTM�� ��������.
	//void MakeAnimation_POS(Animation* _nowAnimtion, ASEObjects* meshObject, int& _posIndex);
	//void MakeAnimation_ROT(Animation* _nowAnimtion, ASEObjects* meshObject, int& _rotIndex);

	// ��Ű�׿� �ʿ��� �Լ�
	void MakeSkinnedNodeTM_VS2(std::vector<DXObjectBase*>& meshObject);
	//void MakeBoneWorldTM(std::vector<ASEObjects*>& meshObject);
#pragma endregion

#pragma region MK3 - Ÿ�Կ� ���������� ���� ����
	/// info�� ase���� �̸��� ������ ���,
	void CreateDXObjects_ASEParserAll(MeshBuildInfo* pmeshInfo, std::vector<DXObjectBase*>& Objects);


	void MakeHierarchy_VS3(std::vector<DXObjectBase*>& meshObject); /// meshObject���� �θ�-�ڽ� ���踦 ������ش�.
	void MakeLocalTM_VS3(std::vector<DXObjectBase*>& meshObject);
	void MakeSkinnedNodeTM_VS3(std::vector<DXObjectBase*>& meshObject);
#pragma endregion

#pragma region MK4 - MK4 �������� �̸� ASEparser�� �ε��� ���� �ִ� ���� // Ÿ�Կ� ���������� ����. 
public:
	void CreateDXObject(std::string IDName, std::vector<DXObjectBase*>& Objects);

private:
	void CreateMeshBuildInfo(std::string fileName, std::string nodeName);

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

