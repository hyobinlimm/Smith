#pragma once
#include "DXObjectBase.h"
#include "CParsingDataClass.h"
#include "Animation.h"
#include "Effects.h"

/// <summary>
/// ASEparser�� �о�� ��Ű�� ������Ʈ
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

	//�������̵� �ǰ� ��������. 
	void Update(HimeCamera* pCamera) override;
	void Render() override;

	void SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB);
	void SetIndexSize(int val) { m_IndexSize = val; }
	void SetTexResource(std::wstring name);

	virtual void SetPos(XMFLOAT4 Pos) override {};
	virtual void SetRot(XMFLOAT4 Rot) override {};

private:
	ID3D11Device* m_pD3dDevice;						// D3D11 ����̽�
	//ID3D11DeviceContext* m_pDeviceContext;		// ����̽� ���ؽ�Ʈ
	ID3D11RasterizerState* m_pRenderstate;

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//���ҽ� ���� ����
	ResourceManager* m_pResourceManager;

	// ��ȯ ����
	//XMFLOAT4X4 m_World;	// ���� ��ȯ ��� (����->����)
	XMFLOAT4X4 m_View;		// �þ� ��ȯ ��� (ī�޶� ��)
	XMFLOAT4X4 m_Proj;		// ���� ��ȯ ��� (����/����)

	int m_IndexSize;

public:
	void SetMaterial(ASEParser::ASEMaterial* materialData);

	XMFLOAT4X4 GetHierarchyNodeTM(NodeInfo* myNode);	// ���� LocalTM * �θ��� LocalTM ���ִ� ����Լ� 
	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }

	void DecomposeLocalTM();

	// �ִϸ��̼��� ������ ���� ��ǥ ���� �Լ�
	virtual void UpdateAnimation(int m_framecount) override {};
	virtual void CalcLocalTM() {};

	void UpdateSkinningBoneTM();

private:
	UINT m_IndexCount;
	Material m_Material;

	/// ���� �ܺο��� �޾ƿ����� ����
	DirectionalLight m_DirLights[3];
	XMFLOAT3 m_EyePosW;
	UINT m_LightCount;

	/// �ؽ��Ŀ��� �߰��� �κ�
	//ID3D11ShaderResourceView* m_pDiffuseMapSRV;

	XMFLOAT4X4 m_TexTransform;

public:

	// �ִϸ��̼��� ���� ���
	//XMVECTOR m_Position;
	//XMVECTOR m_Rotate;
	//XMVECTOR m_Scale;

	// ���� ���� ���� �ִ� ������
	int m_PosIndex;
	int m_RotIndex;

	// ���� �����ϴ� ���� 
	//vector<ASEParser::Bone*> m_BoneList;

	// ��Ű���� TM�� �����ϴ� ������
	XMFLOAT4X4 m_FinalBoneMatrix[96];
};

