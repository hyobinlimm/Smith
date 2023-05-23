#pragma once
#include "DXObjectBase.h"
#include "CParsingDataClass.h"
#include "Animation.h"
#include "Effects.h"

/// <summary>
/// ASEparser로 읽어온 스키닝 오브젝트
/// 
/// </summary>

class ResourceManager;
class HimeCamera;
class MeshObject;

class SkinningObject : public DXObjectBase
{
public:
	SkinningObject(ID3D11RasterizerState* pRS);
	SkinningObject(ID3D11RasterizerState* pRS, XMVECTOR pos);
	~SkinningObject();

public:
	void Initialize();

	//오버라이드 되게 수정하자. 
	void Update(HimeCamera* pCamera) override;
	void Render() override;

	void SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB);
	void SetIndexSize(int val) { m_IndexSize = val; }
	void SetTexResource(std::wstring name);

	virtual void SetPos(XMFLOAT4 Pos) override {};
	virtual void SetRot(XMFLOAT4 Rot) override {};

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
	XMFLOAT4X4 m_View;		// 시야 변환 행렬 (카메라 뷰)
	XMFLOAT4X4 m_Proj;		// 투영 변환 행렬 (원근/직교)

	int m_IndexSize;

public:
	void SetMaterial(ASEParser::ASEMaterial* materialData);

	XMFLOAT4X4 GetHierarchyNodeTM(NodeInfo* myNode);	// 나의 LocalTM * 부모의 LocalTM 해주는 재귀함수 
	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }

	void DecomposeLocalTM();

	// 애니메이션을 돌리기 위한 좌표 변경 함수
	virtual void UpdateAnimation(int m_framecount) override {};
	virtual void CalcLocalTM() {};

	void UpdateSkinningBoneTM();

private:
	UINT m_IndexCount;
	Material m_Material;

	/// 빛은 외부에서 받아오도록 하자
	DirectionalLight m_DirLights[3];
	XMFLOAT3 m_EyePosW;
	UINT m_LightCount;

	/// 텍스쳐에서 추가된 부분
	//ID3D11ShaderResourceView* m_pDiffuseMapSRV;

	XMFLOAT4X4 m_TexTransform;

public:

	// 애니메이션을 위한 행렬
	//XMVECTOR m_Position;
	//XMVECTOR m_Rotate;
	//XMVECTOR m_Scale;

	// 현재 내가 보고 있는 프레임
	int m_PosIndex;
	int m_RotIndex;

	// 본을 관리하는 벡터 
	//vector<ASEParser::Bone*> m_BoneList;

	// 스키닝의 TM를 관리하는 변수들
	XMFLOAT4X4 m_FinalBoneMatrix[96];
};

