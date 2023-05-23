#pragma once

#include "CASEParser.h"
#include "d3dx11Effect.h"
#include "HimeRendererStructs.h"
#include "NodeInfo.h"
#include <map>

/// <summary>
/// 게임에 필요한 모든 리소스를 갖고 있는 객체
/// 그리고 그것을 관리(레퍼런스 카운트가 다 하면 삭제한다던지) 한다.
/// 
/// 딱 한번 생성된다. 
/// 
/// 싱글턴으로 바꿔서 오브젝트들이 갖고 올 수 있도록 하자. 
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

	// collision 위해서 최재영이 만듬.
	void Create_Volume(std::wstring name, ASEParser::GeomObject* meshData);

public: // 밖에서 스키닝 오브젝트을 만들때 필요한 함수들
	void SetBuildInfoAll(std::vector<FilePath*> pFilePath);
	void ChangedBuidInfo(std::wstring FileName); // 와꾸만 있음. 필요할때 구현하겠다. 

	void LoadASEObject(std::wstring name);
	void LoadASEObject_BuildInfo(std::wstring IDName);

private:
	static ResourceManager* m_pResourceManager;
	ID3D11Device* m_D3dDevice;

	CASEParser* m_pASEParser;

public:
	// ASEObject에 필요한 변수
	std::map < std::wstring, NodeInfo* > m_NodeInfoList;

	UINT m_IndexCount;

	/// <summary>
	/// 리소스들의 리스트 (여기가 원본)
	/// </summary>
	std::map<std::wstring, BuildInfo*> m_BuildInfoList;
	std::map<std::wstring, GeometryBuffer* > m_GBufferList;	// 기하 리소스(VB, IB)
	std::map<std::wstring, GeometryBuffer* > m_VBufferList;
	std::map<std::wstring, vector<float>> m_minmaxList;

	// 오브젝트 판별용인데 일단 여기다 넣었다.
	float colX;
	float colZ;
	float colX2;
	float colZ2;
};

