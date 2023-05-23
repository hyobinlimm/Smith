#pragma once
#include "DXObjectBase.h"
#include "CParsingDataClass.h"
#include "Animation.h"
#include "Effects.h"

/// <summary>
/// ASEparser로 읽어온 오브젝트
/// 
/// </summary>

class ResourceManager;
class HimeCamera;

class MeshObject : public DXObjectBase
{
public:
	MeshObject(ID3D11RasterizerState* pRS);
	MeshObject(ID3D11RasterizerState* pRS, XMVECTOR pos);
	~MeshObject();

public:
	void Initialize();
	
	void Update(HimeCamera* pCamera) override;
	void Render() override; 
	void SetTexResource(std::wstring name) override;
	
	void SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB);
	void SetIndexSize(int val) { m_IndexSize = val; }
	void SetMaterial(ASEParser::ASEMaterial* materialData);

private:
	ID3D11Device* m_pD3dDevice;						// D3D11 디바이스
	//ID3D11DeviceContext* m_pDeviceContext;		// 디바이스 컨텍스트
	ID3D11RasterizerState* m_pRenderstate;

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//리소스 관련 변수
	ResourceManager* m_pResourceManager;

	// 변환 관련
	//XMFLOAT4X4 m_World;	// 월드 변환 행렬 (로컬->월드)
	XMFLOAT4X4 m_View;	// 시야 변환 행렬 (카메라 뷰)
	XMFLOAT4X4 m_Proj;	// 투영 변환 행렬 (원근/직교)

	int m_IndexSize;

public: //get set 함수 모음
	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }
	virtual void SetPos(XMFLOAT4 Pos) override;
	virtual void SetRot(XMFLOAT4 Rot) override;

	// LocatTM를 pos, rot, scale로 분해하는 함수
	void DecomposeLocalTM(); 

private:
	// 나의 LocalTM * 부모의 LocalTM 해주는 재귀함수 
	XMFLOAT4X4 GetHierarchyNodeTM(NodeInfo* myNode);

	// 애니메이션을 돌리기 위한 좌표 변경 함수
	void UpdateAnimation(int m_framecount) override;
	virtual void CalcLocalTM() override;

private:
	UINT m_IndexCount;
	Material m_Material;

	/// 빛은 외부에서 받아오도록 하자
	DirectionalLight m_DirLights[3];
	XMFLOAT3 m_EyePosW;

	ID3D11ShaderResourceView* m_pNormalMapSRV;

	XMFLOAT4X4 m_TexTransform;

public:

	// 현재 내가 보고 있는 프레임
	int m_PosIndex;
	int m_RotIndex;

	bool m_IsSkinningObj;
	//라디안 계산용 변수
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);
};

