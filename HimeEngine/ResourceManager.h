#pragma once

#include "CASEParser.h"
#include "d3dx11Effect.h"
#include "HimeRendererStructs.h"
#include "NodeInfo.h"
#include <map>

/// <summary>
/// ���ӿ� �ʿ��� ��� ���ҽ��� ���� �ִ� ��ü
/// �׸��� �װ��� ����(���۷��� ī��Ʈ�� �� �ϸ� �����Ѵٴ���) �Ѵ�.
/// 
/// �� �ѹ� �����ȴ�. 
/// 
/// �̱������� �ٲ㼭 ������Ʈ���� ���� �� �� �ֵ��� ����. 
/// 22.07.07 hyobin
/// </summary>

class GeometryBuffer;
class BuildInfo;

class ResourceManager
{
private:
	ResourceManager();
	~ResourceManager();

public:
	static ResourceManager* GetInst();
	static void DestroyInst();

	void Initialize();
	GeometryBuffer* GetGBuffer(std::wstring name);
	GeometryBuffer* GetVBuffer(std::wstring name);

	//void Input
	CASEParser* GetCASEParser() const { return m_pASEParser; }

private:
	void CreateBuildInfo(std::wstring name, std::wstring ASEName, std::wstring mapName);

	void Create_GridGeometry();
	void Create_AxisGeometry();

	void Create_MeshGeometry(std::wstring name, ASEParser::GeomObject* meshData);
	void Create_NodeInfo_MeshObject(GeomObject* meshData);

	void Create_SkinningMeshGeometry(std::wstring name, ASEParser::GeomObject* meshData);
	void Create_NodeInfo_SkinningObject(ASEParser::GeomObject* meshData);

	void Create_HelperGeometry(std::wstring name, int meshCount, ASEParser::GeomObject* meshData);
	void Create_NodeInfo_Hepler(std::wstring name, int meshCount, ASEParser::GeomObject* meshData);

	void Create_ShapeGeometry(ASEParser::GeomObject* meshData);
	void Create_NodeInfo_Shape(ASEParser::GeomObject* meshData);

	void Create_Animation(CASEParser* pCASEParser, std::wstring name);

	XMFLOAT4X4 CreateNodeTM(ASEParser::GeomObject& meshData);
	XMMATRIX CreateInverseNodeTM(ASEParser::GeomObject& meshData);

	void Delete_ASEParser();

	// collision ���ؼ� ���翵�� ����.
	void Create_Volume(std::wstring name, ASEParser::GeomObject* meshData);

public: // �ۿ��� ��Ű�� ������Ʈ�� ���鶧 �ʿ��� �Լ���
	void SetBuildInfoAll(std::vector<FilePath*> pFilePath);
	void ChangedBuidInfo(std::wstring FileName); // �Ͳٸ� ����. �ʿ��Ҷ� �����ϰڴ�. 

	void LoadASEObject(std::wstring name);
	void LoadASEObject_BuildInfo(std::wstring IDName);

private:
	static ResourceManager* m_pResourceManager;
	ID3D11Device* m_D3dDevice;

	CASEParser* m_pASEParser;

public:
	// ASEObject�� �ʿ��� ����
	std::map < std::wstring, NodeInfo* > m_NodeInfoList;

	UINT m_IndexCount;

	/// <summary>
	/// ���ҽ����� ����Ʈ (���Ⱑ ����)
	/// </summary>
	std::map<std::wstring, BuildInfo*> m_BuildInfoList;
	std::map<std::wstring, GeometryBuffer* > m_GBufferList;	// ���� ���ҽ�(VB, IB)
	std::map<std::wstring, GeometryBuffer* > m_VBufferList;
	std::map<std::wstring, vector<float>> m_minmaxList;

	// ������Ʈ �Ǻ����ε� �ϴ� ����� �־���.
	float colX;
	float colZ;
	float colX2;
	float colZ2;
};

