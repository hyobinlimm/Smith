#pragma once
#include "DXObjectBase.h"
#include "CParsingDataClass.h"
#include "Animation.h"
#include "Effects.h"

/// <summary>
/// ASEparser�� �о�� ������Ʈ
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
	ID3D11Device* m_pD3dDevice;						// D3D11 ����̽�
	//ID3D11DeviceContext* m_pDeviceContext;		// ����̽� ���ؽ�Ʈ
	ID3D11RasterizerState* m_pRenderstate;

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//���ҽ� ���� ����
	ResourceManager* m_pResourceManager;

	// ��ȯ ����
	//XMFLOAT4X4 m_World;	// ���� ��ȯ ��� (����->����)
	XMFLOAT4X4 m_View;	// �þ� ��ȯ ��� (ī�޶� ��)
	XMFLOAT4X4 m_Proj;	// ���� ��ȯ ��� (����/����)

	int m_IndexSize;

public: //get set �Լ� ����
	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }
	virtual void SetPos(XMFLOAT4 Pos) override;
	virtual void SetRot(XMFLOAT4 Rot) override;

	// LocatTM�� pos, rot, scale�� �����ϴ� �Լ�
	void DecomposeLocalTM(); 

private:
	// ���� LocalTM * �θ��� LocalTM ���ִ� ����Լ� 
	XMFLOAT4X4 GetHierarchyNodeTM(NodeInfo* myNode);

	// �ִϸ��̼��� ������ ���� ��ǥ ���� �Լ�
	void UpdateAnimation(int m_framecount) override;
	virtual void CalcLocalTM() override;

private:
	UINT m_IndexCount;
	Material m_Material;

	/// ���� �ܺο��� �޾ƿ����� ����
	DirectionalLight m_DirLights[3];
	XMFLOAT3 m_EyePosW;

	ID3D11ShaderResourceView* m_pNormalMapSRV;

	XMFLOAT4X4 m_TexTransform;

public:

	// ���� ���� ���� �ִ� ������
	int m_PosIndex;
	int m_RotIndex;

	bool m_IsSkinningObj;
	//���� ���� ����
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);
};

