#pragma once
#include "DXObjectBase.h"
#include "CParsingDataClass.h"
#include "Effects.h"

/// <summary>
/// Bounding Volume ����� ���� Ŭ����
/// </summary>

class ResourceManager;
class HimeCamera;

class VolumeObject : public DXObjectBase
{
public:
	VolumeObject(ID3D11RasterizerState* pRS);
	VolumeObject(ID3D11RasterizerState* pRS, XMVECTOR pos);
	~VolumeObject();

public:
	void Initialize();

	void Update(HimeCamera* pCamera) override;
	void Render() override;

	void SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB);
	void SetIndexSize(int val) { m_IndexSize = val; }

	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }
	virtual void SetPos(XMFLOAT4 Pos) override;
	virtual void SetRot(XMFLOAT4 Rot) override;

	// LocatTM�� pos, rot, scale�� �����ϴ� �Լ�
	void DecomposeLocalTM();

	void SetTexResource(std::wstring name) override {};
	virtual void UpdateAnimation(int m_framecount) override {};
	virtual void CalcLocalTM() {};


private:
	ID3D11Device* m_pD3dDevice;						// D3D11 ����̽�
	ID3D11DeviceContext* m_pDeviceContext;		// ����̽� ���ؽ�Ʈ
	ID3D11RasterizerState* m_pRenderstate;

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	XMFLOAT3 m_EyePosW;

	//���ҽ� ���� ����
	ResourceManager* m_pResourceManager;

	// ��ȯ ����
	//XMFLOAT4X4 m_World;	// ���� ��ȯ ��� (����->����)
	XMFLOAT4X4 m_View;	// �þ� ��ȯ ��� (ī�޶� ��)
	XMFLOAT4X4 m_Proj;	// ���� ��ȯ ��� (����/����)

	int m_IndexSize;

private:
	UINT m_IndexCount;
};

